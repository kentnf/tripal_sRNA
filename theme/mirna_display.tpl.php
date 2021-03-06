<?php

drupal_set_title($organism->common_name . " miRNA ". $mirna->mirna_id );

print '<div class="row"> <div class="col-md-8 col-md-offset-2">';

// display miRNA
$target_html = 'NA';
if ($target) {
  $target_html = l('click here', 'mirna/target/' . $mirna->mirna_id); 
} 

$srna_html = l($mirna->srna_id, 'srna/display/' . $mirna->srna_id);

$head_data = array('Sequence', 'Length', 'miRNA family', 'sRNA ID', 'Target');
$rows_data = array(array( $mirna->sequence, $mirna->length, $mirna->family, $srna_html, $target_html));
$variables = array(
  'header' => $head_data,
  'rows' => $rows_data,
  'attributes' => array('class' => 'table vertical-align'),
);
print theme('table', $variables);

// display expression
print '<br><h1 class="page-header">Digital expression</h1>';
$head_data = array('Organism', 'Cultivar', 'Tissue', 'Total reads', '# of sRNA reads');
$rows_data = array();
foreach ($expression as $e) {
 
  $experiment = chado_generate_var('experiment', array('experiment_id' => $e->experiment_id));
  $experiment = chado_expand_var($experiment, 'table', 'experimentprop', array('return_array' => 1));
  $experiment->properties = new stdClass;

  // append prop to experiment
  foreach ($experiment->experimentprop as $prop) {
    if (!is_null($prop->value)) {
      $cvterm_name = $prop->type_id->name;
      $experiment->properties->$cvterm_name = $prop->value;
    }
  }

  //append biosample prop to experiment
  $biosample = $experiment->biomaterial_id;
  $biosample = chado_expand_var($biosample, 'table', 'biomaterialprop', array('return_array' => 1));

  foreach ($biosample->biomaterialprop as $prop) {
    if (!is_null($prop->value)) {
      $cvterm_name = $prop->type_id->name;
      $experiment->properties->$cvterm_name = $prop->value;
    }
  }
  $experiment->properties->number_count = $e->number_count;

  $row = array(
    l($organism->common_name , $organism->organism_id),
    property_exists($experiment->properties, 'cultivar') ? $experiment->properties->cultivar : '',
    property_exists($experiment->properties, 'tissue') ? $experiment->properties->tissue : '',
    property_exists($experiment->properties, 'total_count') ? $experiment->properties->total_count : '',
    property_exists($experiment->properties, 'number_count') ? $experiment->properties->number_count : '',
  );
  $rows_data[] = $row;
}
$variables = array(
  'header' => $head_data,
  'rows' => $rows_data,
  'attributes' => array('class' => 'table vertical-align'),
);
print theme('table', $variables);

// Hits on known miRNAs
print '<br><h1 class="page-header">Hits on known miRNAs</h1>';
if (count($conserved) > 0) {
  $head_data = array('hit miRNA', 'alignment');
  $rows_data = array();
  foreach ($conserved as $c) {
    $alignment = '<pre>' . $c->align_query . "\n" . $c->align_string . "\n" . $c->align_hit . '</pre>';
    $row = array($c->hit_id, $alignment);
    $rows_data[] = $row;
  }
  $variables = array(
    'header' => $head_data,
    'rows' => $rows_data,
    'attributes' => array('class' => 'table vertical-align'),
  );

  $table = theme('table', $variables);
  print '<div class="row"> <div class="col-md-8 col-md-offset-2">' . $table  . '</div> </div>';
} else {
  print '<div class="page-header"><font color=#FF0000 size=4>No hits on known miRNAs were found</font></div>';
}

// Precursor of miRNA $mirna_id



print '<br><h1 class="page-header">Precursor of miRNA ' . $mirna->mirna_id . '</h1>';
if (count($hairpin) > 0) {
  $head_data = array('sequence ID', 'start', 'end', 'strand', 'sequence', 'folding energy', 'miRNA*', 'structure');
  $rows_data = array();
  foreach ($hairpin as $h) {
    $sql = "SELECT * FROM {mirna_star} WHERE hairpin_id=:hairpin_id";
    $star_html = '';
    $result = db_query($sql, array(':hairpin_id'=>$h->hairpin_id))->fetchField();
    if ($result) {
      $star_html = l('miRNA*', 'mirna/star/' . $h->hairpin_id . "/". $mirna->mirna_id);
    } 
    $stucture_html = l('structure', 'mirna/structure/' . $h->hairpin_id);
    $sequence_html = '<pre>';
    $s = str_split($h->sequence, 1);
    $n = 0;
    foreach ($s as $str) {
      ++$n;
      if ($n == $h->mir_start) {
        $sequence_html.= "<font color=#FF0000>";
      }
      $sequence_html.= $str;
      if ($n == $h->mir_end) {
        $sequence_html.= "</font>";
      }
      if ($n % 30 == 0) {
        $sequence_html.= "\n";
      }
    }

    $row = array(
      $h->genome_id, $h->hit_start, $h->hit_end, $h->strand, 
      $sequence_html, $h->folding_energy, $star_html, $stucture_html
    );
    $rows_data[] = $row;
  }
  $variables = array(
    'header' => $head_data,
    'rows' => $rows_data,
    'attributes' => array('class' => 'table vertical-align'),
  );
  print theme('table', $variables);
} else {
  print '<div class="page-header"><font color=#FF0000 size=4>No precusor were found</font></div>';
}

print '</div></div>';
