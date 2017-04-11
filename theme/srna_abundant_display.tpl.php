<?php

$title = 'Top ' . $top . ' highly abundant sRNAs for '. $sample;
drupal_set_title($title);
 
if (count($data) > 0) {
  $header_data = array('sRNA ID', 'Sequence', 'Length', 'Annotation', 'number of count');
  $rows_data = array();
  $header = array('data'=>$header_data);
  $rows = array('data'=>$rows_data);

  foreach ($data as $record) {
    $line = array_values($record);
    $line[0] = l( $line[0], 'srna/display/' . $line[0]);
    $rows_data[] = $line;
  }

  $variables = array(
    'header' => $header_data,
    'rows' => $rows_data,
    'attributes' => array('class' => 'table vertical-align'),
  );
  print theme('table', $variables);
}
else 
{
  print '<h3>No highly abundant sRNA</h3>';
}


