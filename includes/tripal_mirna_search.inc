<?php
/**
 * load sRNA into database
 *
 */

/**
 * 
 */
function tripal_mirna_search_form($form, $form_state) {

  // drupal_set_title(t('New title for the node form'));

  $org_opts = tripal_srna_get_organism_options();
  $form['organism_id'] = array(
    '#type' => 'select',
    '#title' => 'Select Genome',
    '#options' => $org_opts,
    '#required' => TRUE,
  );

  $form['submit3'] = array(
    '#type' => 'submit',
    '#value' => t('List of all putative miRNAs of selected Genome'),
    '#submit' => array('tripal_mirna_search_form_submit'),
  );

  $form['mirna_id'] = array(
    '#type' => 'textfield',
    '#title' => t('<br> OR <br><br>Input the miRNA ID:'),
    '#maxlength' => 50,
    '#default_value' => t('P_M00001'),
    '#suffix' => t('<div>(current range: P_M00001 - P_M00838)</div>'),
  );

  $form['submit1'] = array(
    '#type' => 'submit',
    '#value' => t('Search by ID'),
    '#submit' => array('tripal_mirna_search_form_submit'),
  );

  $form['mirna_name'] = array(
    '#type' => 'textfield',
    '#title' => t('<br> OR <br><br>Input a miRNA name:'),
    '#maxlength' => 50,
    '#default_value' => t('miR172')
  );

  $form['submit2'] = array(
    '#type' => 'submit',
    '#value' => t('Search by Name'),
    '#submit' => array('tripal_mirna_search_form_submit'),
  );

  $form['#prefix'] = '<div class="row"> <div class="col-md-8 col-md-offset-2">';
  $form['#suffix'] = '</div> </div>';

  return $form;
}

/**
 * 
 */
function tripal_mirna_search_form_validate($form, &$form_state) {

  $organism_id = $form_state['values']['organism_id'];

  if ($form_state['values']['op'] == $form_state['values']['submit1']) {
    if (empty($form_state['values']['mirna_id'])) {
      form_set_error('mirna_id', t('Please input miRNA ID.'));
    } 
  }
  elseif ($form_state['values']['op'] == $form_state['values']['submit2']) {
    if (empty($form_state['values']['mirna_name'])) {
      form_set_error('mirna_name', t('Please input miRNA name.'));
    }
  }
  elseif ($form_state['values']['op'] == $form_state['values']['submit3']) {
    if (empty($form_state['values']['organism_id'])) {
      form_set_error('organism_id', t('No genome selected. Choose a genome from the list.'));
    }
  } 
  else {
    form_set_error('submit1', t('submit button error 1.'));
    form_set_error('submit2', t('submit button error 2.'));
  }
}

/**
 *  
 */
function tripal_mirna_search_form_submit($form, $form_state) {

  $organism_id = $form_state['values']['organism_id'];
  $result = array(); // search result

  // search miRNA by ID
  if ($form_state['values']['op'] == $form_state['values']['submit1']) { 
    // === search public ===
    $mirna_id = $form_state['values']['mirna_id'];
    $sql = "SELECT mirna_id FROM {mirna_feature} WHERE mirna_id = :mirna_id";
    $record = db_query($sql, array(':mirna_id' => $mirna_id))->fetchField();
    if ($record) {
      $result[] = $record;
    } else {
      drupal_set_message(t('Can not find miRNA ID : :mirna_id.', array(':mirna_id'=>$mirna_id)), 'warning');
    }

    // === search chado === 
    //$values =  array(
    //  'uniquename' => $form_state['values']['mirna_id'],
    //  'type_id' => array ('name' => 'miRNA'),
    //);
    //$result = chado_select_record('feature', array('feature_id', 'uniquename'), $values);
  }
  // search miRNA by name
  elseif ( $form_state['values']['op'] == $form_state['values']['submit2'] )  {
    // === search public ===
    $family = $form_state['values']['mirna_name'];
    $sql = "SELECT mirna_id FROM {mirna_feature} WHERE family = :family";
    $record = db_query($sql, array(':family' => $family));
    foreach ($record as $r) {
      $result[] = $r->mirna_id;
    }

    // === search chado ===
    //$values =  array( 
    //  'name' => $form_state['values']['mirna_name'],
    //  'type_id' => array ('name' => 'miRNA'),
    //);
    //$result = chado_select_record('feature', array('feature_id', 'uniquename'), $values);
  }
  // list all miRNA for one species 
  elseif ( $form_state['values']['op'] == $form_state['values']['submit3'] ) {
    // === search public ===
    $sql = "SELECT mirna_id FROM {mirna_feature} WHERE organism_id = :organism_id";
    $record = db_query($sql, array(':organism_id' => $organism_id));
    foreach ($record as $r) {
      $result[] = $r->mirna_id;
    }

    // === search chado ===
    //$values =  array(
    //  'organism_id' => $organism_id,
    //  'type_id' => array ('name' => 'miRNA'),
    //);
    //$result = chado_select_record('feature', array('feature_id', 'uniquename'), $values);
  }

  if (count($result) > 1) {
    // display miRNA search result list
    $_SESSION['mirna_list'] = $result;
    drupal_goto("mirna/list");
  } 
  elseif (count($result) == 1) {
    // display miRNA search result single
    drupal_goto("mirna/display/" . $result[0]);
  } 
  else {
    drupal_set_message('No miRNA found.', 'warning'); 
  }
}

/**
 * display miRNA search result
 * 
 * @param $feature_id
 *
 * @return $result
 *  Return HTML of search result
 */
function tripal_mirna_search_display($uniquename) {

  $path = drupal_get_path('module', 'tripal_srna');
  drupal_add_css($path . '/theme/css/report.css');

  // uniquename in chado is miRNA_id in public
  // get miRNA 
  $sql = "SELECT f.mirna_id, s.sequence, s.length, f.family, s.srna_id, s.organism_id FROM {mirna_feature} f
    LEFT JOIN {srna_feature} s ON f.srna_id = s.srna_id
    WHERE mirna_id = :mirna_id";
  $mirna = db_query($sql, array(':mirna_id'=>$uniquename))->fetchObject();

  $result = chado_select_record('organism', array('common_name'),array('organism_id'=> $mirna->organism_id));
  $organism = $result[0]->common_name;

  // get miRNA expression
  $sql = "SELECT s.*, e.number_count FROM {srna_feature_expression} e
    LEFT JOIN {srna_sample} s ON s.sample_id = e.sample_id
    WHERE e.srna_id = :srna_id";
  $expression = array();
  $exp = db_query($sql, array(':srna_id'=>$mirna->srna_id));
  foreach ($exp as $e) { $expression[] = $e; }

  // get miRNA conserved
  $sql = "SELECT * FROM {mirna_conserved} WHERE mirna_id = :mirna_id";
  $conserved = array();
  $conserved_query = db_query($sql, array(':mirna_id' => $uniquename));
  foreach ($conserved_query as $c) { $conserved[] = $c; }

  // get hairpin
  $sql = "SELECT * FROM {mirna_hairpin} WHERE mirna_id = :mirna_id"; 
  $hairpin = array();
  $hairpin_query = db_query($sql, array(':mirna_id' => $uniquename)); 
  foreach ($hairpin_query as $h) { $hairpin[] = $h; } 

  // target
  $sql = "SELECT * FROM {mirna_target} WHERE mirna_id = :mirna_id";;
  $target = db_query($sql, array(':mirna_id' => $uniquename))->fetchField();

  /** 
   * === for chado (not finished) ===
  // get miRNA feature
  $feature = chado_generate_var('feature', array('uniquename' => $uniquename));
  $sql = "SELECT f.residues, f.seqlen, f.name, s.srna_id FROM {chado.feature} f 
    LEFT JOIN {srna_feature} s ON f.residues = s.sequence AND f.organism_id = s.organism
    WHERE f.uniquename = :uniquename";
  $srna = db_query($sql, array(':uniquename'=>$uniquename))->fetchObject();

  // get digital expression
  $sql = "SELECT p.cultivar, p.tissue, p.total_count, e.number_count FROM {srna_feature_expression} e
    LEFT JOIN {srna_sample} p ON e.sample_id = p.sample_id
    WHERE e.srna_id = :srna_id";
  $exp = db_query($sql, array(':srna_id'=>$srna->srna_id)); 

  //foreach ($exp as $e) {
  //  $sample_exp[] = $sample;
  //}
  */

  return theme('mirna_display', 
    array(
      'mirna' => $mirna, 'target' => $target, 'expression' => $expression, 
      'conserved' => $conserved, 'hairpin' => $hairpin, 'organism' => $organism
    )
  );
}

/**
 * list miRNAs
 *
 */
function tripal_mirna_search_list() {

  $path = drupal_get_path('module', 'tripal_srna');
  drupal_add_css($path . '/theme/css/report.css');

  $list = $_SESSION['mirna_list'];

  $sql1 = "SELECT f.mirna_id, s.sequence, s.length, e.number_count FROM {mirna_feature} f
    LEFT JOIN {srna_feature} s ON f.srna_id = s.srna_id
    LEFT JOIN {srna_feature_expression} e ON s.srna_id = e.srna_id
    WHERE mirna_id = :mirna_id
    ORDER BY e.number_count DESC LIMIT 1";

  $sql2 = "SELECT * FROM {mirna_target} WHERE mirna_id = :mirna_id";
  
  $mirnas = array();
  foreach ($list as $mirna_id) {
    // miRNA
    $mirna = db_query($sql1, array(':mirna_id' => $mirna_id))->fetchObject();
    // target
    $target = db_query($sql2, array(':mirna_id' => $mirna_id))->fetchField();
    $mirnas[] = array($mirna, $target);
  }

  return theme('mirna_list', array('mirnas' => $mirnas));
}

function tripal_mirna_search_target($mirna_id) {

  $path = drupal_get_path('module', 'tripal_srna');
  drupal_add_css($path . '/theme/css/report.css');

  $sql = "SELECT * FROM {mirna_target} WHERE mirna_id = :mirna_id";
  $targets = array();
  $targets_query = db_query($sql, array(':mirna_id' => $mirna_id));

  foreach ($targets_query as $t) {
    $targets[] = $t;
  }

  return theme('mirna_target', array('targets' => $targets));
}

/**
 * Display miRNA star sequence
 * 
 * @param hairpin_id, mirna_id
 *
 * @return HTML of star
 */
function tripal_mirna_search_star($hairpin_id, $mirna_id) {

  $path = drupal_get_path('module', 'tripal_srna');
  drupal_add_css($path . '/theme/css/report.css');

  $sql = "SELECT * FROM {mirna_hairpin} WHERE hairpin_id = :hairpin_id AND mirna_id = :mirna_id";
  $hairpin = db_query($sql, array(':hairpin_id'=>$hairpin_id, ':mirna_id'=>$mirna_id))->fetchObject();

  $sql = "SELECT * FROM {mirna_star} WHERE hairpin_id = :hairpin_id";
  $stars = array();
  $star_query = db_query($sql, array(':hairpin_id' => $hairpin_id));

  foreach ($star_query as $s) {
    $stars[] = $s;
  }

  return theme('mirna_star', array('hairpin' => $hairpin, 'stars' => $stars));
}

/**
 * Display pre-miRNA structure
 * 
 * @param hairpin_id
 *
 * @return HTML of hairpin structure
 */
function tripal_mirna_search_structure($hairpin_id) {

  return theme('mirna_structure', array('image' => $hairpin_img));
}
