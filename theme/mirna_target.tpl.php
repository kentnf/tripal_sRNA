<?php

dpm($targets);

drupal_set_title("Predicted targets of miRNA " . $targets[0]->mirna_id);

$head_data = array('ID', 'start', 'end', 'strand', 'score', 'alignment');
$rows_data = array();
foreach ($targets as $target) {
  $mRNA_html = $target->target_id;

  // query chado for mRNA id
  $feature = chado_generate_var('feature', array('uniquename'=>$target->target_id));
 
  if (isset($feature->nid)) {
    $mRNA_html = l($target->target_id, 'node/' . $feature->nid);
  }

  $alignment = '<pre>';
  $alignment.= "mRNA  5' " . $target->align_query .  " 3'\n";
  $alignment.= "         " . $target->align_string . "\n";
  $alignment.= "miRNA 3' " . $target->align_hit .    " 5'\n";
  $alignment.= '</pre>';

  $row = array(
    $mRNA_html, 
    $target->align_start, 
    $target->align_end, 
    $target->strand, 
    $target->score,
    $alignment
  );

  $rows_data[] = $row;
}

$variables = array(
  'header' => $head_data,
  'rows' => $rows_data,
  'attributes' => array('class' => 'table vertical-align'),
);
print '<div class="row"> <div class="col-md-8 col-md-offset-2">';
print theme('table', $variables);
print '</div></div>';

