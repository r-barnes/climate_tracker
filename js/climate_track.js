//ScaleBar: http://trac.osgeo.org/openlayers/wiki/Addins/ScaleBar
//Distinct colours: http://stackoverflow.com/questions/2103368/color-logic-algorithm
/////////////////////////////////
//Globals
/////////////////////////////////

//My colours
var trackcolours=['Blue', 'Green', 'Red', 'Orange', 'Aqua', 'Chartreuse',
'Brown', 'Salmon', 'Purple', 'MediumSpringGreen', 'Yellow'];

//Colorbrewer Colours
//var trackcolours=['#A6CEE3', '#1F78B4', '#B2DF8A', '#33A02C', '#FB9A99',
//'#E31A1C', '#FDBF6F', '#FF7F00', '#CAB2D6', '#6A3D9A', '#FFFF99'];

var tracklist=new Array();
var track_hashes=new Array();
var $j=jQuery.noConflict();
var SERVER_URL="./server.php";
var Fit_station_str;
var Fit_surf_params;
var Fit_box=null;      //Holds vector object square denoting selected region

/////////////////////////////////
//Map Object
/////////////////////////////////
var map;

/////////////////////////////////
//Base Layer
/////////////////////////////////
var wms_north_america;

var wms;

/*var wms_states = new OpenLayers.Layer.WMS("States","http://mrdata.usgs.gov/cgi-bin/mapserv?",{map: 'usstates.map', transparent: 'true', layers: 'states'}, {singleTile: 'true', ratio: 1, isBaseLayer:'true'});
map.addLayer(wms_states);*/

/////////////////////////////////
//Stations Layer
/////////////////////////////////

var station_style;
var clustering;
var stations;

var click;
var selcontrol;
var tracks;

function display_stations(){
  var count=0;
  document.getElementById("stationslist").options.length=0;
  for(i in stations.selectedFeatures)
    for(x in stations.selectedFeatures[i].cluster){
      AddItem("stationslist",stations.selectedFeatures[i].cluster[x].attributes.description,0);
      count++;
    }

  if(document.getElementById("stationslist").options.length>0){
    $j('#fit_submit').prop('disabled', false);
    $j('#clear_fit').prop('disabled', false);
  } else {
    $j('#fit_submit').prop('disabled', true);
    $j('#clear_fit').prop('disabled', true);
  }

  $j('#fitprocessing').html("");
  $j('#stationnum').html(count);
}

function onFeatureSelect(evt) {
  display_stations();
}

function onFeatureUnselect(evt) {
  display_stations();
}

function station_highlighted(e){
  var hover_stations="";
  var hovercount=0;
  for(s in e.feature.cluster){
    hover_stations+="<li>"+e.feature.cluster[s].attributes.description+"</li>";
    hovercount++;
  }
  $j('#hoverlist').html(hover_stations);
  $j('#hovernum').html(hovercount);
}

function station_unhighlighted(e){
  $j('#hoverlist').html("");
  $j('#hovernum').html(0);
}

/////////////////////////////////
//Track Layer
/////////////////////////////////
var TrackFilter = new OpenLayers.Filter.Comparison({
  type: OpenLayers.Filter.Comparison.LESS_THAN_OR_EQUAL_TO,
  property: "when",
  value: 2011,
});

TrackFilterStrategy = new OpenLayers.Strategy.Filter({filter: TrackFilter});

function AddItem(ListBox,Text,Value,Color){
  // Create an Option object        
  var opt = document.createElement("option");

  // Add an Option object to Drop Down/List Box
  document.getElementById(ListBox).options.add(opt);
  // Assign text and value to Option object
  opt.text = Text;
  opt.value = Value;
  if(Color)
    opt.style.backgroundColor=Color;
}

function do_track_hashes(){
  temp="";
  for(i in track_hashes)
    if(track_hashes[i]!=null)
      temp+=track_hashes[i]+",";
  $j('#track_hashes').val(temp);
}

var trackcount=0;
function add_to_tracklist(track_name,lat,lon,warning){
  var item="";
  var num=tracklist.length-1;
  trackcount++;
  item+="<div id=\"track" + num + "\" style=\"float:left;width:100%;background-color:" + trackcolours[num%trackcolours.length] + "\">"
  item+="<a onclick=\"remove_track(" + num + ")\"><img src=\"img/trash.png\" width=\"16\" height=\"16\"></a>";
  if(warning)
    item+="<img src=\"img/warning.png\" width=\"16\" height=\"16\"> ";
  item+="<a onclick=\"switch_velocity(" + num + ")\"><img src=\"img/bounce_arrow.png\" width=\"16\" height=\"16\" title=\"Calculate Velocity\"></a>";
  item+=track_name+" (";
  item+=(lat).toFixed(2);
  item+=", "+(lon).toFixed(2);
  item+=")</div>";
  $j('#tracknum').html(trackcount);
  $j('#tracklist').append(item);

  do_track_hashes();
}

function remove_track(num){
  trackcount--;
  tracks.removeFeatures(tracklist[num]);
  $j('#track'+num).remove();
  $j('#tracknum').html(trackcount);
  track_hashes[num]=null;

  do_track_hashes();
}

function process_incoming_track(track_name,track_hash,years,lon,lat,in_reverse){
  var stylepoint = { strokeColor: trackcolours[tracklist.length%trackcolours.length],//'#0000ff', 
    strokeOpacity: 0.6,
    pointRadius: 5
  };
  //Todo: Consider creating a number of sub-lines rather than line segments by creating a continuously-expanding array of points
  var detrack=new Array();
  var maxdist=0;

  var startpointloc=null;
  if(in_reverse)
    startpointloc=new OpenLayers.Geometry.Point(parseFloat(lon[lon.length-1]),parseFloat(lat[lat.length-1]));
  else
    startpointloc=new OpenLayers.Geometry.Point(parseFloat(lon[0]),parseFloat(lat[0]));


  var startpoint=new OpenLayers.Feature.Vector(startpointloc, null, stylepoint);
  startpoint.attributes.when=1900;//parseFloat(years[0]);
  detrack.push(startpoint);
  tracks.addFeatures([startpoint]);
  for(i=1;i<lat.length;i++){
    var point1=new OpenLayers.Geometry.Point(lon[i-1],lat[i-1]);
    point1.lon=lon[i-1];
    point1.lat=lat[i-1];
    var point2=new OpenLayers.Geometry.Point(lon[i],lat[i]);
    point2.lon=lon[i];
    point2.lat=lat[i];
    var intermediate_point={lon:point2.lon,lat:point1.lat};
    var dist = OpenLayers.Util.distVincenty(point1,point2);
    var ndist = OpenLayers.Util.distVincenty(intermediate_point,point2);
    var edist = OpenLayers.Util.distVincenty(point1,intermediate_point);
    if(point2.lon>point1.lon) //If point2 is to the West
      edist=-edist;
    if(point2.lat<point1.lat) //If point2 is to the South
      ndist=-ndist;
    if(dist>maxdist)
      maxdist=dist;
    var line = new OpenLayers.Geometry.LineString([point1,point2]);
    var lineFeature=new OpenLayers.Feature.Vector(line, null,
      {strokeColor: trackcolours[tracklist.length%trackcolours.length],//'#0000ff', 
        strokeOpacity: 1,//0.6,
        strokeWidth: 8//5
      }
    );
    lineFeature.attributes.when=parseFloat(years[i]);
    lineFeature.attributes.dist=dist;
    lineFeature.attributes.ndist=ndist;
    lineFeature.attributes.edist=edist;
    detrack.push(lineFeature)
    tracks.addFeatures([lineFeature]);
  }
  var warning=false;
  if(maxdist>100){
    warning=true;
    $j('#trackprocessing').html('<img src="img/warning.png">');
  }
  tracklist.push(detrack);
  track_hashes.push(track_hash);
  add_to_tracklist(track_name,parseFloat(lat[0]),parseFloat(lon[0]),warning);
}

var Track_Params;
var Track_time;
function Track_Handler(request){
  $j('#trackprocessing').prop('title',(new Date().getTime()-Track_time)/1000);
  if(request.status!=200 || request.responseText.substring(0,5)=="Error"){
    $j('#trackprocessing').html('<img src="img/bad.gif" width="16" height="16">');
    $j('#track_submit').removeClass("disabled");
    $j('#stations_submit').removeClass("disabled");
    $j('#clear_fit').prop('disabled',false);
    click.activate();
    return;
  }

  $j('#track_submit').removeClass("disabled");
  $j('#stations_submit').removeClass("disabled");
  $j('#clear_fit').prop('disabled',false);
  click.activate();
  $j('#trackprocessing').html('<img src="img/good.gif" width="16" height="16">');

  try{
    var track_response=$j.parseJSON(request.responseText);
  } catch (err) {
    $j('#trackprocessing').html('<img src="img/bad.gif" width="16" height="16">');
    $j('#track_submit').removeClass("disabled");
    $j('#stations_submit').removeClass("disabled");
    $j('#clear_fit').prop('disabled',false);
    click.activate();
    return;
  }

  for(i in track_response){
    tr=track_response[i];
    track_name=tr['trackprcp'].toFixed(1) + "\" and " + tr['tracktemp'].toFixed(1) + "&deg;";
    process_incoming_track(track_name,tr['trackhash'],tr['year'],tr['lon'],tr['lat'],tr['in_reverse']);
  }
}


function Grad_Handler(request){
  if(request.status!=200 || request.responseText.substring(0,5)=="Error"){
    $j('#trackprocessing').html('<img src="img/bad.gif" width="16" height="16">');
    $j('#track_submit').removeClass("disabled");
    $j('#stations_submit').removeClass("disabled");
    $j('#clear_fit').prop('disabled',false);
    click.activate();
    return;
  }

  $j('#track_submit').removeClass("disabled");
  $j('#stations_submit').removeClass("disabled");
  $j('#clear_fit').prop('disabled',false);
  click.activate();
  $j('#trackprocessing').html('<img src="img/good.gif" width="16" height="16">');

  try{
    var grad_response=$j.parseJSON(request.responseText);
  } catch (err) {
    $j('#trackprocessing').html('<img src="img/bad.gif" width="16" height="16">');
    $j('#track_submit').removeClass("disabled");
    $j('#stations_submit').removeClass("disabled");
    $j('#clear_fit').prop('disabled',false);
    click.activate();
    return;
  }

  $j('#grad_values').html("Pdx: " + grad_response[0].pvaldx + ", Pdy: " + grad_response[0].pvaldy + ", Tdx: " +  grad_response[0].tvaldx + ", Tdy: " + grad_response[0].tvaldy);
}



function TrackHashLoad(){
  if($j('#interesting_surfaces').val()=="box"){
    var hashes=$j('#track_hashes').val();
    hashes=hashes.split(",");
    for(i in hashes){
      var request = OpenLayers.Request.POST({
        url: SERVER_URL,
        params: {"type":"TrackHashLoad","hash":$j.trim(hashes[i])},
        headers: {"Content-Type": "text/plain"},
        callback: Track_Handler
      });
    }
  } else {
    var request = OpenLayers.Request.POST({
      url: SERVER_URL,
      params: {"type":"TrackHashLoad","hash":$j('#interesting_surfaces').val()},
      headers: {"Content-Type": "text/plain"},
      callback: Track_Handler
    });
  }
}

var xurfaces_time;
function FitSurfaces_Handler(request){
  $j('#fitprocessing').prop('title',(new Date().getTime()-FitSurfaces_time)/1000);
  if(request.status!=200 || request.responseText.substring(0,5)=="Error"){
    $j('#fitprocessing').html("<img src=\"img/bad.gif\" width=\"16\" height=\"16\">");
    $j('#fit_submit').removeClass("disabled");
    $j('#clear_fit').prop('disabled',false);
    return;
  }

  Fit_station_str=request.responseText;
  $j('#clear_fit').prop('disabled',false);
  $j('#track_submit').removeClass("disabled");
  $j('#track_submit').addClass("down");
  doTrack();
  $j('#fitprocessing').html("<img src=\"img/good.gif\" width=\"16\" height=\"16\">");
}

function FitSurfaces(){
  $j('#fit_submit').prop('disabled',true);
  $j('#clear_fit').prop('disabled',true);
  click.deactivate();
  $j('#fitprocessing').html("<img src=\"img/processing.gif\" width=\"16\" height=\"16\">");

  if(Fit_box!=null){
    tracks.removeFeatures([Fit_box]);
    Fit_box=null;
  }

  Fit_station_str="";
  var bounds=null;
  for(i in stations.selectedFeatures)
    for(x in stations.selectedFeatures[i].cluster){
      Fit_station_str+=","+stations.selectedFeatures[i].cluster[x].attributes.title;
      if(bounds==null)
        bounds=stations.selectedFeatures[i].cluster[x].geometry.getBounds().clone();
      else
        bounds.extend(stations.selectedFeatures[i].cluster[x].geometry.getBounds());
    }

  var FitBounds=bounds.toGeometry();
  Fit_box=new OpenLayers.Feature.Vector(FitBounds, null, {
        strokeColor: "#0033ff",
        strokeOpacity: 0.7,
        strokeWidth: 2,
        fillOpacity: 0.4,
        fillColor: "white"
      });
  Fit_box.attributes.when=1900;
  tracks.addFeatures([Fit_box]);

  FitSurfaces_time=new Date().getTime();
  Fit_surf_params={"type":"FitSurfaces","data":Fit_station_str,"bounds":bounds.toString(),"surface_type":$j('#surface_type').val(),"do_seasonal":$j('#do_seasonal').val()};
  var request = OpenLayers.Request.POST({
    url: SERVER_URL,
    params: Fit_surf_params,
    headers: {"Content-Type": "text/plain"},
    callback: FitSurfaces_Handler
  });
}

/////////////////////////////////
//GUI Controls
/////////////////////////////////

function doPan(){
  selcontrol.deactivate();
  click.deactivate();
  $j("#pan_submit").addClass("down");
  $j("#stations_submit").removeClass("down");
  $j("#track_submit").removeClass("down");
}

function doSelect(){
  if($j("#stations_submit").hasClass("disabled")) return;
  selcontrol.activate();
  click.deactivate();
  $j("#track_submit").addClass("disabled");
  $j("#pan_submit").removeClass("down");
  $j("#stations_submit").addClass("down");
  $j("#track_submit").removeClass("down");
}

function doTrack(){
  if($j("#track_submit").hasClass("disabled")) return;
  click.activate();
  selcontrol.deactivate();
  $j("#pan_submit").removeClass("down");
  $j("#stations_submit").removeClass("down");
  $j("#track_submit").addClass("down");
}

function ClearStations(){
  selcontrol.unselectAll();
  $j("#track_submit").addClass("disabled");
  $j("#track_submit").removeClass("down");
  click.deactivate();
  $j('#fitprocessing').html("");
  doSelect();
}

function doBackTrack(){
  if($j("#backtrack_submit").hasClass("down"))
    $j("#backtrack_submit").removeClass("down");
  else
    $j("#backtrack_submit").addClass("down");
}

function doGradient(){
  if($j("#gradient_submit").hasClass("down"))
    $j("#gradient_submit").removeClass("down");
  else
    $j("#gradient_submit").addClass("down");
}

function QuickSelect(){
  var qslist=quick_stations[$j('#quickstations').val()];
  selcontrol.unselectAll();
  for(i in stations.features)
    for(s in stations.features[i].cluster)
      if(qslist.indexOf(parseInt(stations.features[i].cluster[s].attributes.title))!=-1)
        stations.features[i].cluster[s].attributes.selected=1;
      else
        stations.features[i].cluster[s].attributes.selected=0;
  clustering.recluster();
  for(i in stations.features)
    for(s in stations.features[i].cluster)
      if(qslist.indexOf(parseInt(stations.features[i].cluster[s].attributes.title))!=-1){
        selcontrol.select(stations.features[i]);
        break;
      }
}

function centerMN(){
  map.setCenter(new OpenLayers.LonLat(-93.9196,45.7326),6);
}

function centerUS(){
  map.setCenter(new OpenLayers.LonLat(-98.58333,39.833333),4);
}

var map_year=2011;
function change_map_year(year){
  year=parseInt(year);
  if(!(1900<=year && year<=2011)) return;
  map_year=year;
  $j('#year_range_value').html(map_year);
  $j('#year_range').slider( "value", map_year );
  document.title="Climate Tracker (" + map_year + ")";
  TrackFilter.value=map_year;
  TrackFilterStrategy.setFilter(TrackFilter);
}

var animate_speed=1;
var animate_pid=null;

function animator(){
  var new_animate_speed=parseFloat($j('#animatespeed').val());
  var year=map_year+1;
  console.info(year);
  if(new_animate_speed!=animate_speed){
    animate_speed=new_animate_speed;
    window.clearInterval(animate_pid);
    animate_pid=window.setInterval(animator,animate_speed*1000);
  }
  if(year>2011 && $j('#animateloop').prop('checked'))
    year=1900;
  else if (year>2011 && !$j('#animateloop').prop('checked')){
    window.clearInterval(animate_pid);
    $j("#animate_submit").removeClass("down");
    return;
  }
  change_map_year(year);
}

function Animate(){
  if($j("#animate_submit").hasClass("down")){  //We are already animating
    window.clearInterval(animate_pid);
    $j("#animate_submit").removeClass("down");
  } else {
    animate_pid=window.setInterval(animator,animate_speed*1000);
    $j("#animate_submit").addClass("down");
  }
}

//////////////////////////////////////
//VELOCITY
//////////////////////////////////////

var velocity_track=-1;
var old_colour;

function undraw_velocity(){
  if(velocity_track==-1) return;

  old_colour=trackcolours[velocity_track%trackcolours.length];

  for(i in tracklist[velocity_track]){
    tracklist[velocity_track][i].style.strokeColor=old_colour;
    tracklist[velocity_track][i].style.strokeOpacity=0.6;
  }
}

function calculate_velocity(){
  if (velocity_track==-1) return;
  tracklist[velocity_track][1].style.strokeColor="Black";
  var velocity_min=parseInt($j('#velocity_min_num').html());
  var velocity_max=parseInt($j('#velocity_max_num').html());
  old_colour=trackcolours[velocity_track%trackcolours.length];
  var speed_avg=0;
  var nspeed_avg=0;
  var espeed_avg=0;
  var segs=0;
  for(i in tracklist[velocity_track]){
    line_when=tracklist[velocity_track][i].attributes.when;
    if(velocity_min<=line_when && line_when<=velocity_max){
      tracklist[velocity_track][i].style.strokeColor="Black";
      tracklist[velocity_track][i].style.strokeOpacity=1.0;
      if(!isNaN(tracklist[velocity_track][i].attributes.dist)){
        speed_avg+=tracklist[velocity_track][i].attributes.dist;
        nspeed_avg+=tracklist[velocity_track][i].attributes.ndist;
        espeed_avg+=tracklist[velocity_track][i].attributes.edist;
        segs++;  
      }
    } else {
      tracklist[velocity_track][i].style.strokeColor=old_colour;
      tracklist[velocity_track][i].style.strokeOpacity=0.6;
    }
  }
  $j('#velocity_avg').html("Avg: " + (speed_avg/segs/1.6).toFixed(2) + " miles");
  $j('#velocity_avg').append("&nbsp;&nbsp;N_Avg: " + (nspeed_avg/segs/1.6).toFixed(2) + " miles");
  $j('#velocity_avg').append("&nbsp;&nbsp;E_Avg: " + (espeed_avg/segs/1.6).toFixed(2) + " miles");
}

function switch_velocity(track){
  if(velocity_track!=track)
    undraw_velocity();
  velocity_track=track;
  calculate_velocity();
  tracks.redraw();
}

function change_velocity_min(year){
  $j('#velocity_min_num').html(year);
  switch_velocity(velocity_track);
}

function change_velocity_max(year){
  $j('#velocity_max_num').html(year);
  switch_velocity(velocity_track);
}

var fit_hide=false;
function ToggleFixBox(){
  if(fit_hide)
    tracks.addFeatures([Fit_box]);
  else
    tracks.removeFeatures([Fit_box]);
  fit_hide=!fit_hide;
}

/////////////////////////////////
//Initialise
/////////////////////////////////
function init(){
  var stateResetSettings = { //TODO: Changing this doesn't seem to affect anything
    east__size:       300,
    east__initClosed:	true,
    east__initHidden:	true,
  };

  var myLayout;
  myLayout = $j('body').layout({
	  east__minSize:      100,
    center__minWidth:   500,
    east__showOverflowOnHover: true,
    stateManagement__enabled:	 true // automatic cookie load & save enabled by default
  });
  myLayout;

  map = new OpenLayers.Map('map',{projection: new OpenLayers.Projection("EPSG:4326")});

  wms = new OpenLayers.Layer.WMS( "OpenLayers WMS", 
    "http://vmap0.tiles.osgeo.org/wms/vmap0",
    {layers: 'basic', isBaseLayer:true, enableLocalCache:true},
    {isBaseLayer:'true'}
  );
  map.addLayer(wms);

  slabel = new OpenLayers.Layer.WMS( "State Labels", 
    "http://vmap0.tiles.osgeo.org/wms/vmap0",
    {transparent:true, layers: 'statelabel', enableLocalCache:true}
  );
  map.addLayer(slabel);

  roads = new OpenLayers.Layer.WMS( "Roads", 
    "http://vmap0.tiles.osgeo.org/wms/vmap0",
    {transparent:true, layers: 'priroad,secroad', enableLocalCache:true}
  );
  roads.setVisibility(false);
  map.addLayer(roads);

  ctylabel = new OpenLayers.Layer.WMS( "City Label", 
    "http://vmap0.tiles.osgeo.org/wms/vmap0",
    {transparent:true, layers: 'ctylabel', enableLocalCache:true}
  );
  ctylabel.setVisibility(false);
  map.addLayer(ctylabel);

  var station_style = new OpenLayers.StyleMap({
    "default": new OpenLayers.Style({
        pointRadius: "7", // sized according to type attribute
        fillColor: "#ffcc66",
        strokeColor: "#ff9933",
        strokeWidth: 2,
        graphicZIndex: 1,
      fillOpacity:0.5
    }),
    "select": new OpenLayers.Style({
        fillColor: "#66ccff",
        strokeColor: "#3399ff",
        graphicZIndex: 2
    })
  });


  clustering = new OpenLayers.Strategy.AttributeCluster({attribute:"selected"});//AttributeCluster({attribute:'selected'})
  stations = new OpenLayers.Layer.Vector("Stations", {
    styleMap: station_style,
    strategies: [
      new OpenLayers.Strategy.Fixed(),
      clustering
    ],
    protocol: new OpenLayers.Protocol.HTTP({
      url: "./stations.dat",
      format: new OpenLayers.Format.Text({extractStyles:false})
    })
  });
  map.addLayer(stations);

  stations.events.on({
        'featureselected': onFeatureSelect,
      'featureunselected': onFeatureUnselect
  });

  tracks = new OpenLayers.Layer.Vector("Tracks",{
    strategies: [TrackFilterStrategy], //new OpenLayers.Strategy.Fixed() ?
    rendererOptions: { zIndexing: true }
  });

  map.addLayer(tracks);


  OpenLayers.Control.Click = OpenLayers.Class(OpenLayers.Control, {                
    defaultHandlerOptions: {
        'single': true,
        'double': false,
        'pixelTolerance': 0,
        'stopSingle': false,
        'stopDouble': false
    },

    initialize: function(options) {
        this.handlerOptions = OpenLayers.Util.extend(
            {}, this.defaultHandlerOptions
        );
        OpenLayers.Control.prototype.initialize.apply(
            this, arguments
        ); 
        this.handler = new OpenLayers.Handler.Click(
            this, {
                'click': this.trigger
            }, this.handlerOptions
        );
    }, 

    trigger: function(e) {
      click.deactivate();
      $j('#track_submit').addClass("disabled");
      $j('#stations_submit').addClass("disabled");
      $j('#clear_fit').prop('disabled',true);
      $j('#trackprocessing').html('<img src="img/processing.gif" width="16" height="16">');
      Track_time=new Date().getTime();
        var lonlat = map.getLonLatFromViewPortPx(e.xy);
      TrackParams=Fit_surf_params;
      if( $j("#gradient_submit").hasClass("down") ){
        TrackParams.type="Gradient";
        TrackParams.callback=Grad_Handler;
      }else{
        TrackParams.type="Track";
        TrackParams.callback=Track_Handler;
      }

      TrackParams.surf=Fit_station_str;
      TrackParams.x=lonlat.lon;
      TrackParams.y=lonlat.lat;
      TrackParams.backtrack=$j("#backtrack_submit").hasClass("down");
      TrackParams.year=$j('#year_range').slider("value");
      var request = OpenLayers.Request.POST({
        url: SERVER_URL,
        params: TrackParams,
        headers: {"Content-Type": "text/plain"},
        callback: TrackParams.callback
      });
    }

  });

  click=new OpenLayers.Control.Click()
  map.addControl(click);

  selcontrol = new OpenLayers.Control.SelectFeature(stations,{
    clickout: false, toggle: true,
    multiple: false, hover: true, highlightOnly: true,
    toggleKey: "ctrlKey", // ctrl key removes from selection
    multipleKey: "shiftKey", // shift key adds to selection
    box: true,
    eventListeners: {featurehighlighted: station_highlighted,
            featureunhighlighted: station_unhighlighted}
  });
  map.addControl(selcontrol);

  //Map Year
  $j('#year_range').slider({
    min:1900,
    max:2011,
    value: 2011,
    slide: function( event, ui ) {
      change_map_year(ui.value)
    }
  });
  $j('#year_range').slider();


  // Year range
  $j('#vrange_slider').slider({
    range: true,
    min:1900,
    max:2011,
    values: [1900, 2011],
    slide: function( event, ui ) {
      change_velocity_min(ui.values[0]);
      change_velocity_max(ui.values[1]);
    }
  });


  map.setCenter(new OpenLayers.LonLat(-93.9196,45.7326),5);
  map.addControl(new OpenLayers.Control.LayerSwitcher());
  map.addControl(new OpenLayers.Control.MousePosition({numDigits:3}));
  map.addControl(new OpenLayers.Control.ScaleBar({
            divisions: 2,   // default is 2
            subdivisions: 2, // default is 2
            displaySystem: 'english'
          }));
  for(i in quick_stations)
    AddItem("quickstations",quick_stations[i][0],i);
}
