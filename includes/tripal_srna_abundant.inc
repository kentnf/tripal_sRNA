<?php
/**
 * Identify highly abundant sRNAs
 *
 */

/**
 * 
 */
function tripal_srna_abundant_form($form, $form_state) {

  $sample_opts = tripal_srna_get_sample_options();

  $form['sample_id'] = array(
    '#type' => 'select',
    '#title' => 'Select a sample',
    '#options' => $sample_opts,
    '#required' => TRUE,
  );

  $form['top'] = array(
    '#type' => 'textfield',
    '#title' => t('Top:'),
    '#default_value' => 50,
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t(' Submit '),
  );

  $form['#prefix'] = '<div class="row"> <div class="col-md-8 col-md-offset-2">';
  $form['#suffix'] = '</div> </div>';

  return $form;
}

/**
 * 
 */
function tripal_srna_abundant_form_validate($form, &$form_state) {

  if (empty($form_state['values']['sample_id'])) {
    form_set_error('sample_id', t('Choose a sample from the list.'));
  }

  if ((empty($form_state['values']['top']))) {
    form_set_error('top', t('Please input a integer > 1.'));
  } else {
    if ($form_state['values']['top'] < 1 ) {
      form_set_error('top', t('Please input a integer > 1.'));
    }
  }
}

/**
 *  
 */
function tripal_srna_abundant_form_submit($form, $form_state) {
  $sample_id = $form_state['values']['sample_id'];
  $top = $form_state['values']['top'];

  $sql = "SELECT e.srna_id, f.sequence, f.length, f.annotation, e.number_count FROM {srna_feature_expression} e 
     LEFT JOIN {srna_feature} f ON e.srna_id = f.srna_id 
     WHERE e.sample_id = :sample_id 
     ORDER BY e.number_count DESC
     LIMIT :top;
  ";
  $result = db_query($sql, array(':sample_id' => $sample_id, ':top'=>$top));
  $data = array();
  foreach ($result as $record) {
    $array = (array) $record;
    //dpm($array);
    $data[] = $array;
  }

  // session can store array, but not class
  $_SESSION['srna_abundant_data'] = $data;
  $_SESSION['srna_abundant_top'] = $top;
  $_SESSION['srna_abundant_sample'] = $sample_id;
  drupal_goto("srna/abundant/display");
}

/**
 * display sRNA abundant result including sequence and expression
 * 
 * @param NA
 *
 * @return 
 *  Return HTML of srna_abundant_display
 */
function tripal_srna_abundant_display() {
  return theme('srna_abundant_display', array(
    'data' => $_SESSION['srna_abundant_data'], 
    'top' => $_SESSION['srna_abundant_top'],
    'sample' => $_SESSION['srna_abundant_sample'],
  ));
}

