<?php
/**
 * load sRNA into database
 */

/**
 * 
 */
function tripal_srna_sample_loader_form($form, &$form_state) {

  $org_opts = tripal_srna_get_organism_options();

  $form['organism_id'] = array(
    '#type' => 'select',
    '#title' => 'Select Genome',
    '#options' => $org_opts,
    '#required' => TRUE,
  );

  $form['sample_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Sample ID'),
    '#maxlength' => 50,
    '#required' => TRUE,
  );

  $form['cultivar'] = array(
    '#type' => 'textfield',
    '#title' => t('Cultivar'),
    '#maxlength' => 50,
  );
  $form['tissue'] = array(
    '#type' => 'textfield',
    '#title' => t('Tissue'),
    '#maxlength' => 50,
  );
  $form['stage'] = array(
    '#type' => 'textfield',
    '#title' => t('Stage'),
    '#maxlength' => 50,
  );

  $form['treatment'] = array(
    '#type' => 'textfield',
    '#title' => t('Treatement'),
    '#maxlength' => 50,
  );

  $form['total_num'] = array(
    '#type' => 'textfield',
    '#title' => t('Total number of cleaned small RNA reads'),
    '#maxlength' => 50,
    '#required' => TRUE,
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Sample Description'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#default_value' => ' Submit ',
  );

  return $form;
}

/**
 * 
 */
function tripal_srna_sample_loader_form_validate($form, &$form_state) {

  if (empty($form_state['values']['organism_id'])) {
    form_set_error('organism_id', t('No genome selected. Choose a genome from the list.'));
  }

  $sample_id = $form_state['values']['sample_id']; 
  $result = db_query('SELECT sample_id FROM {srna_sample} WHERE sample_id=:sample_id ', array(':sample_id' => $sample_id))->fetchField();

  if(!empty($result)) {
    form_set_error('sample_id', t('the sample id exist in database.'));
  } 
}

/**
 * submit tripal job 
 */
function tripal_srna_sample_loader_form_submit($form, &$form_state) {

  $sample_id   = $form_state['values']['sample_id'];
  $organism_id = $form_state['values']['organism_id'];
  $cultivar    = $form_state['values']['cultivar'];
  $tissue      = $form_state['values']['tissue']; 
  $stage       = $form_state['values']['stage'];
  $treatment   = $form_state['values']['treatment'];
  $total_num   = $form_state['values']['total_num'];
  $description = $form_state['values']['description'];

  // insert samle to database
  try { 
    $insert = db_insert('srna_sample')->fields(array(
      'sample_id'   => $sample_id,
      'organism_id' => $organism_id,
      'cultivar'    => $cultivar,
      'tissue'      => $tissue,
      'stage'       => $stage,
      'treatment'   => $treatment,
      'description' => $description,
      'total_count' => $total_num,
    ))->execute();
    drupal_set_message(t('sample '.$sample_id.' has been insert into database.'), 'status');
  }
  catch (Exception $e) {
    drupal_set_message(t('Error: %message', array('%message' => $e->getMessage())), 'error');
    tripal_report_error('tripal_srna', TRIPAL_ERROR, "Can not load record to srna_sample" . $e);
  }
}


