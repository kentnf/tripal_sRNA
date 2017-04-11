<?php

$head_data = array('miRNA ID', 'sequence', 'length', 'largest count', 'target');
$rows_data = array();

foreach ($mirnas as $mir) {
  $mirna = $mir[0];
  $target = $mir[1];

  $mirna_id = l($mirna->mirna_id, 'mirna/display/' . $mirna->mirna_id);
  $target_html = 'NA';
  if ($target) {
    $target_html = l('click here', 'mirna/target/' . $mirna->mirna_id);  
  }

  $row = array($mirna_id, $mirna->sequence, $mirna->length, $mirna->number_count, $target_html);
  $rows_data[] = $row; 
}
$variables = array(
  'header' => $head_data,
  'rows' => $rows_data,
  'attributes' => array('class' => 'table vertical-align'),
);
print theme('table', $variables);

