<?php
/**
 * Display the miRNA target prediction result
 */

// display title
drupal_set_title(t( $organism->common_name . ' small RNA ' . $srna->srna_id));

print '<div class="row"> <div class="col-md-8 col-md-offset-2">';

//$menu = tripal_srna_submenu();
//drupal_set_breadcrumb($menu);

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
    l($organism->common_name, 'organism/' . $organism->organism_id),
    property_exists($sample->properties, 'cultivar') ? $sample->properties->cultivar : '',
    property_exists($sample->properties, 'tissue') ? $sample->properties->tissue : '',
    property_exists($sample->properties, 'total_count') ? $sample->properties->total_count : '',
    property_exists($sample->properties, 'number_count') ? $sample->properties->number_count : '',
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

print '</div></div>';

