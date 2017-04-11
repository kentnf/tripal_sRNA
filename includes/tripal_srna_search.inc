<?php
/**
 * load sRNA into database
 *
 */

/**
 * 
 */
function tripal_srna_search_form($form, $form_state) {

  // drupal_set_title(t('New title for the node form'));

  // CSS support to the form
  $form['#attached']['css'] = array(
    drupal_get_path('module', 'tripal_srna') . '/theme/css/form.css',
  );

  $org_opts = tripal_srna_get_organism_options();
  $form['organism_id'] = array(
    '#type' => 'select',
    '#title' => 'Select Genome',
    '#options' => $org_opts,
    '#required' => TRUE,
  );

  $form['srna_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Input the small RNA ID:'),
    '#maxlength' => 50,
    '#default_value' => t('P_S0000001'),
    '#suffix' => t('<div>(current range: P_S0000001 - P_S4105095)</div>'),
  );

  $form['submit1'] = array(
    '#type' => 'submit',
    '#value' => t('Search by ID'),
    '#submit' => array('tripal_srna_search_form_submit'),
  );

  $form['srna_seq'] = array(
    '#type' => 'textfield',
    '#title' => t('<br> OR <br><br>Input the small RNA sequence:'),
    '#maxlength' => 50,
    '#default_value' => t('UCCAAAGGGAUCGCAUUGAUC')
  );

  $form['submit2'] = array(
    '#type' => 'submit',
    '#value' => t('Search by Seq'),
    '#submit' => array('tripal_srna_search_form_submit'),
  );

  $form['#prefix'] = '<div class="row"> <div class="col-md-8 col-md-offset-2">';
  $form['#suffix'] = '</div> </div>';

  return $form;
}

/**
 * 
 */
function tripal_srna_search_form_validate($form, &$form_state) {

  if (empty($form_state['values']['organism_id'])) {
    form_set_error('organism_id', t('No genome selected. Choose a genome from the list.'));
  }
  $organism_id = $form_state['values']['organism_id'];

  if ($form_state['values']['op'] == $form_state['values']['submit1']) {
    if (empty($form_state['values']['srna_id'])) {
      form_set_error('srna_id', t('Please input small RNA ID.'));
    } else {
      $srna_id = $form_state['values']['srna_id'];
      $sql = "SELECT srna_id FROM {srna_feature} WHERE organism_id = :organism_id AND srna_id = :srna_id";
      $record = db_query($sql, array(':organism_id' => $organism_id, ':srna_id' => $srna_id))->fetchField();
      if ($record) {
        $form_state['values']['record'] = $record;
      } else {
        form_set_error('srna_id', t('Can not find small RNA ID : :srna_id.', array(':srna_id'=>$srna_id)));
      }
    }
  }
  elseif ($form_state['values']['op'] == $form_state['values']['submit2']) {
    if (empty($form_state['values']['srna_seq'])) {
      form_set_error('srna_seq', t('Please input small RNA sequence.'));
    } else {
      $srna_seq = $form_state['values']['srna_seq'];
      $sql = "SELECT srna_id FROM {srna_feature} WHERE organism_id = :organism_id AND sequence = :srna_seq";
      $record = db_query($sql, array(':organism_id' => $organism_id, ':srna_seq' => $srna_seq))->fetchField();
      if ($record) {
        $form_state['values']['record'] = $record;
      } else {
        form_set_error('srna_seq', t('Can not find small RNA sequence: :srna_seq.', array(':srna_seq'=>$srna_seq)));
      }
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
function tripal_srna_search_form_submit($form, $form_state) {
  $record = $form_state['values']['record'];
  drupal_goto("srna/display/$record");
}

/**
 * display sRNA search result including sequence and expression
 * 
 * @param $srna_id
 *
 * @return $result
 *  Return HTML of search result
 */
function tripal_srna_search_display($record) {
  $sql = "SELECT * FROM {srna_feature} WHERE srna_id = :srna_id";
  $srna = db_query($sql, array(':srna_id'=>$record))->fetchObject();

  $sql = "SELECT * FROM {srna_feature_expression} WHERE srna_id = :srna_id";
  $exp = db_query($sql, array(':srna_id'=>$record));

  $result = chado_select_record('organism', array('common_name'),array('organism_id'=> $srna->organism_id));
  $organism = $result[0]->common_name;

  $sql = "SELECT * FROM {srna_sample} WHERE sample_id = :sample_id";
  $sample_exp = array();
  foreach ($exp as $e) {
    $sample = db_query($sql, array(':sample_id'=>$e->sample_id))->fetchObject();
    $sample->number_count = $e->number_count;
    $sample_exp[] = $sample;
  }

  return theme('srna_display', array('srna' => $srna, 'exp' => $sample_exp, 'organism'=>$organism));
}

