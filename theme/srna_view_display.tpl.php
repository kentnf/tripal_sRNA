<?php

// determine if method is exist
if ($_SESSION['tripal_sirnaview_search']['METHOD']) {

  $jb_data     = $_SESSION['tripal_sirnaview_search']['data'];
  $jb_location = $_SESSION['tripal_sirnaview_search']['location'];
  $jb_tracks   = $_SESSION['tripal_sirnaview_search']['tracks'];
  $tracks = 'DNA,genes,' . implode(",", $_SESSION['tripal_sirnaview_search']['tracks']);

  //dpm($jb_data);
  //dpm($jb_location);
  //dpm($tracks);
  unset($_SESSION['tripal_sirnaview_search']['METHOD']);

  $jbrowse_frame = "<iframe style=\"border: 1px solid rgb(80, 80, 80);\" src=\"/JBrowse/?data=icugi_data/json/$jb_data&loc=$jb_location&tracks=$tracks&tracklist=0&nav=1&overview=0\" height=\"650\" width=\"850\" name=\"jbrowse_iframe\">
  <p>Your browser does not support iframes.</p> </iframe>";

  print '<div class="row"> <div class="col-md-8 col-md-offset-2">';
  print $jbrowse_frame;
  print '</div></div>';
}

