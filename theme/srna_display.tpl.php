<?php
/**
 * Display the miRNA target prediction result
 */

// display title
drupal_set_title(t( $organism . ' small RNA ' . $srna->srna_id));

// display sRNA
$header_data = array('Sequence', 'Length', 'Annotation');
$rows_data = array(array($srna->sequence, $srna->length, $srna->annotation));
$header = array('data' => $header_data);
$rows = array('data' => $rows_data);

$variables = array(
  'header' => $header_data,
  'rows' => $rows_data,
  'attributes' => array('class' => 'table vertical-align'),
);
print theme('table', $variables);

// display expression
print '<br><h1 class="page-header">Digital expression</h1>';

$header_data = array('Organism', 'Cultivar', 'Tissue', 'Total reads', '# of sRNA reads');
$rows_data = array();
foreach ($exp as $sample) {
  $row = array(
    $organism,
    $sample->cultivar,
    $sample->tissue,
    $sample->total_count,
    $sample->number_count
  );
  $rows_data[] = $row;
}

$header = array('data' => $header_data);
$rows = array('data' => $rows_data);

$variables = array(
  'header' => $header_data,
  'rows' => $rows_data,
  'attributes' => array('class' => 'table vertical-align'),
);
print theme('table', $variables);
