<?php
  putenv('MPLCONFIGDIR=temp');

	function HashSurfaceList($list){
		return sha1($list);
	}

	function PrepareSurfaceList(){
		$data=$_REQUEST['data'];
		$data=explode(',',$data);
		sort($data);
		$data=implode("\n",$data);
		$data=trim($data)."\n";
		return $data;
	}

	function FitSurfaces(){
		$list=PrepareSurfaceList();
		$stations=HashSurfaceList($list);

		//Do we have the list of stations ready to input into average-calculator?
		if(!file_exists("products/$stations.stations")){
			$fout=fopen("products/$stations.stations","w");
			if($fout===false){
				print "Error: Unable to open stations list";
				return false;
			}
			fwrite($fout,$list);
			fclose($fout);
		}

		//Have we already constructed the averages for these stations/season?
		if(!file_exists("temp/$stations.temp")){
			exec("./do_monthly_temp.exe -L 1890 -U 2011 -P temp/$stations -a products/$stations.stations -s data/ushcn_monthly/ushcn-stations.txt -m data/ushcn_monthly/9641C_201112_F52.avg -c data/ahccd/temp/TempStations.csv", $output, $ret);
//			exec("./do_gdd -L 1890 -P temp/$stations -a products/$stations.stations -s ushcn_daily/ushcn-stations.txt -d ushcn_daily/us_daily.txt", $output, $ret);
//			exec("./do_hdd -L 1890 -P temp/$stations -a products/$stations.stations -s ushcn_daily/ushcn-stations.txt -d ushcn_daily/us_daily.txt", $output, $ret);
			if($ret!=0){
				print "Error: Failed to extract temperature data.";
				return false;
			}
		}

		if(!file_exists("temp/$stations.prcp")){
			exec("./do_monthly_prcp.exe -L 1890 -U 2011 -P temp/$stations -a products/$stations.stations -s data/ushcn_monthly/ushcn-stations.txt -m data/ushcn_monthly/9641C_201112_F52.pcp -c data/ahccd/prcp/PrcpStations.csv", $output, $ret);
			if($ret!=0){
        print "Error: Failed to extract precipitation data.";
				return false;
      }
		}

		if(!file_exists("products/$stations.surfaces")){
			exec("./ctrack fit products/$stations temp/$stations.PRCP temp/$stations.TAVG", $output, $ret);
			if($ret!=0){
				print "Error: Failed to fit surfaces!\n";
				print "Tried: products/$stations temp/$stations.PRCP temp/$stations.TAVG\n";
				return false;
			}
		}

		if(!file_exists("products/$stations.fits_json")){
      print "Error: Failed to retrieve cached fits.";
      return false;
    }

    $fits_json=file_get_contents("products/$stations.fits_json");
    print '{"stations":"'.$stations.'","fits":'.$fits_json.'}';
		return true;
	}

	function HashSurfaceLoc($stations,$x,$y){
		return sha1($stations.$x.$y);
	}

	function DoTrack($stations,$x,$y,$backtrack){
		$hash=HashSurfaceLoc($stations,$x,$y);

		if($backtrack)
			$track="backtrack";
		else
			$track="track";

		if(!file_exists("products/$hash.track")){
			exec("./ctrack $track products/$stations.surfaces products/$hash.track $x $y",$output,$ret);
			if($ret!=0)
				return false;
		}

		return file_get_contents("products/$hash.track");
	}

	function Track(){
    $_REQUEST=json_decode(file_get_contents('php://input'), true);
		$stations=$_REQUEST['surf'];

		if($_REQUEST['backtrack']=='true')
			$backtrack=true;
		else
			$backtrack=false;

		if(!(is_numeric($_REQUEST['x']) && is_numeric($_REQUEST['y']))){
			print "Error: Non-numeric point";
			return false;
		}
		$x=(float)$_REQUEST['x'];
		$y=(float)$_REQUEST['y'];

		if($track=DoTrack($stations,$x,$y,$backtrack)){
			print "[$track]";
		} else {
			print "Error: Failed to track intersection of yearly surfaces";
			return false;
		}

		return true;
	}


	function DoGradient($stations,$x,$y,$year){
		$hash=HashSurfaceLoc($stations,$x,$y);

		if(!file_exists("products/$hash.$year.grad")){
			exec("./ctrack gradient products/$stations.surfaces products/$hash.$year.grad $x $y $year",$output,$ret);
			if($ret!=0)
				return false;
		}

		return file_get_contents("products/$hash.$year.grad");
	}


	function Gradient(){
    $_REQUEST=json_decode(file_get_contents('php://input'), true);

		$stations=$_REQUEST['surf'];
		$year=$_REQUEST['year'];

		if($_REQUEST['backtrack']=='true')
			$backtrack=true;
		else
			$backtrack=false;

		if(!(is_numeric($_REQUEST['x']) && is_numeric($_REQUEST['y']))){
			print "Error: Non-numeric point";
			return false;
		}
		$x=(float)$_REQUEST['x'];
		$y=(float)$_REQUEST['y'];

		if($track=DoGradient($stations,$x,$y,$year)){
			print "[$track]";
		} else {
			print "Error: Failed to track intersection of yearly surfaces";
			return false;
		}

		return true;
	}






	function Contours(){
		$surf=$_REQUEST['surf'];

		if(!file_exists("products/$surf.contours")){
			exec("./ctrack contours products/$surf.surfaces products/$surf.contours",$output,$ret);
			if($ret!=0)
				print "Error: Failed to run contour program.";
		}

		print file_get_contents("products/$surf.contours");
	}







	function TrackHashLoad(){
		$hash=$_REQUEST['hash'];
		if (preg_match ('/[^a-z0-9._]/i', $hash)) {	//Check to see if string is clean
			print "Error: Load string is potentially unclean.";
			return;
		}
		if(file_exists("products/$hash")){
			$fin=file_get_contents("products/$hash");
			print "[$fin]";
		} else
			print "Error: Track file not found.";
		return;
	}

  var_dump(file_get_contents('php://input'), true);
  $contents = ob_get_contents();
  ob_end_clean();
  error_log($contents);

	if($_REQUEST['type']=='FitSurfaces')
		FitSurfaces();
	elseif ($_REQUEST['type']=='Track')
		Track();
	elseif ($_REQUEST['type']=='Gradient')
		Gradient();
	elseif ($_REQUEST['type']=='TrackHashLoad')
		TrackHashLoad();
	elseif ($_REQUEST['type']=='Contours')
		Contours();
	else
		print "Error: Unrecognised command";
?>
