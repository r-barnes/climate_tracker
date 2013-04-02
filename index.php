<html>
	<head>
		<title>Climate Tracker</title>
<?php
  if($_GET['local']){
?>
    <script src="js/jquery-1.9.1.js"></script>
    <script src="js/jquery-ui-1.10.1.custom.min.js"></script>
<?php } else { ?>
    <script src="//ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js"></script>
    <script src="//ajax.googleapis.com/ajax/libs/jqueryui/1.10.1/jquery-ui.min.js"></script>
<?php } ?>

    <script src="js/jquery.layout.min-1.3.0.js"></script>

    <script src="openlayers/OpenLayers.js"></script>
    <script src="openlayers/OpenLayers_AddOns.js"></script>
		<script src="openlayers/OpenLayers_ScaleBar.js"></script>

		<script src="js/station_data.js"></script>
		<script src="js/climate_track.js" defer></script>

		<link type="text/css" href="css/ui-lightness/jquery-ui-1.10.1.custom.min.css" rel="stylesheet">
    <link type="text/css" rel="stylesheet" href="css/layout_style.css">
		<link rel="stylesheet" href="css/style.css" type="text/css">
  </head>
  <body onLoad="init()">
	<div class="ui-layout-center" id="main">
		<div id="nav" style="width:100%; height:500px; float:left">
			<div id="controls">
				<a class="button down" id="pan_submit" onClick="doPan()">Pan/Zoom Map</a>
				<a class="button" id="stations_submit" onClick="doSelect()">Select Stations</a>
				<a class="button disabled" id="track_submit" onClick="doTrack()">Track</a>
				<a class="button" id="backtrack_submit" onClick="doBackTrack()">Backtrack</a>
				<a class="button" id="gradient_submit" onClick="doGradient()">Gradient</a>
				<span id="trackprocessing" style="margin-right:5em;"></span>
				<select id="which_season" style="display:none"><option>Summer</option><option>Winter</option></select>

				<div style="float:right">
					<a class="button" id="animate_submit" onClick="Animate()">Animate</a>
					<select id="animatespeed"><option value="0.5">Fast</option><option value="1" selected>Slow</option></select>
					<label>Loop<input type="checkbox" id="animateloop"></label>
				</div>
				<div style="float:right;margin-right:2em;">
          <a class="button disabled" id="doContours" onClick="doContours()">Contours</a>
					<input type="submit" value="Center MN" onClick="centerMN()">
					<input type="submit" value="Center US" onClick="centerUS()">
				</div>
			</div>
			<div id="mapwrapper" style="margin:5px 0 5px 0;width:100%;position:relative;">
				<div id="year_range_value">2011</div>
				<div style="width:100%; height:600px; border:1px solid black;" id="map"></div>
			</div>
			<div id="year_range_div" style="text-align:center;width:100%;height:5em">
				<div id="year_range" style="width:90%;margin:10px auto 0 auto"></div>
				<div id="vrange_slider" style="width:90%;margin:10px auto 0 auto"></div>
				<span id="velocity_min_num" style="float:left">1900</span>
				<span id="velocity_max_num" style="float:right">2011</span>
				<span id="velocity_avg" style="margin-left:2em;">Avg: </span>
			</div>

			<span id="grad_values" style="margin-left:2em;"></span>

			<p><img src="img/warning.png">This sign indicates that two sequential points in the track were more than ~62.5 miles apart. This may indicate a bad fit.</p>
			<p>The year in the upper right denotes the upper year of the bracket of data being shown.</p>
			<p>Note that stations are "clustered". Selecting a station icon may select one or more neighbouring stations. Presently, stations don't preserve their selection colouring when you zoom in/out.</p>

			<p>Created by Richard. Last updated 02013-03-30 at 12:39CST.</p>
		</div>
	</div>

	<div class="ui-layout-east" id="sidebar">
    <img style="position: fixed; top: 0; right: 0; border: 0;" src="https://s3.amazonaws.com/github/ribbons/forkme_right_green_007200.png" usemap="forkme_map">
    <map name="forkme_map">
      <area shape="poly" coords="16,0,56,0,148,92,148,132" href="https://github.com/r-barnes/climate_tracker" alt="Fork me on GitHub">
    </map>


		<b>Selected Stations (<span id="stationnum">0</span>)</b>
		<br><select id="stationslist" size="10" style="width:100%">
		</select>
		<br>
		<input type="submit" id="fit_submit" value="Fit Surfaces" onClick="FitSurfaces()" disabled="true">
		<input type="submit" id="clear_fit" value="Clear" onClick="ClearStations()" disabled="true">
		<input type="submit" id="fit_box_toggle" value="Box" onClick="ToggleFitBox()">
		<br><select id="surface_type" style="display:none;"><option>Linear</option><option selected>Quad</option></select>
		<select id="do_seasonal" onchange="year_season_select(this.value)" style="display:none"><option selected>Yearly</option><option>Seasonal</option></select>
		<br>
		<select id="quickstations">
		</select>
		<input type="submit" id="quick_submit" value="<-Select" onClick="QuickSelect()">
		<span id="fitprocessing"></span>
		<hr>
		<b>Track List (<span id="tracknum">0</span>)</b><input type="submit" id="track_hash_load" onClick="TrackHashLoad()" value="Load">
		<select id="interesting_surfaces" style="width:100%">
			<option value="box">From Box Below</option>
<!--				<option value="9b2f3aec1d109ca7a47d1172dc5b451c4fa1727c.quad.yearly">Twin Cities</option>
			<option value="c7d18e8b54657eec65b20b5dd64ba743abe4182f.quad.yearly">Red River Valley</option>
			<option value="c8a89c3d652540684e63157fbe7d846107a2f248.quad.yearly">Duluth</option>
			<option value="fc6fd50e36c9e2337b4a91476f030c4e2769d9d0.quad.yearly">Rochester</option>
			<option value="65340d4a4c30e00482fc68ff19bd165ed9694ee9.quad.yearly">Marshall</option>-->
		</select>
		<br><input id="track_hashes" type="text" style="width:100%">
		<br><div id="tracklist" style="height:10em;border:1px solid black;overflow-y:scroll;">
		</div>
		<hr>
		<b>Hovering Over (<span id="hovernum">0</span>)</b>
		<ul id="hoverlist"></ul>
	</div>
</body>
</html>
