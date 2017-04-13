<?php

// determine if method is exist
if ($_SESSION['tripal_sirnaview_search']['METHOD']) {

  $jb_data     = $_SESSION['tripal_sirnaview_search']['data'];
  $jb_location = $_SESSION['tripal_sirnaview_search']['location'];
  $jb_tracks   = $_SESSION['tripal_sirnaview_search']['tracks'];
  $tracks = implode(",", $_SESSION['tripal_sirnaview_search']['tracks']);

  // data for test
  $jb_data = 'melon_v351';
  $jb_location = 'MELO3C026828';
  $jb_tracks = 'DNA,gene';

  unset($_SESSION['tripal_sirnaview_search']['METHOD']);

  $jbrowse_frame = "<iframe style=\"border: 1px solid rgb(80, 80, 80);\" src=\"/JBrowse/?data=icugi_data/json/$jb_data&loc=$jb_location&tracks=$tracks&tracklist=0&nav=1&overview=0\" height=\"650\" width=\"850\" name=\"jbrowse_iframe\">
  <p>Your browser does not support iframes.</p> </iframe>";

  print $jbrowse_frame;
}

