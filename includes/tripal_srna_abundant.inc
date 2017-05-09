<?php
/**
 * Identify highly abundant sRNAs
 *
 */

/**
 * 
 */
function tripal_srna_abundant_form($form, $form_state) {

  $org_opts = tripal_srna_get_organism_options();
  $form['reference'] = array(
    '#type' => 'fieldset',
    '#title' => t('Select Reference Genome'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#description' => 'the refernece genome is only used in analyzing small RNAs, not the speicies or cultivar of small RNA sample',
  );

  $form['reference']['organism_id'] = array(
    '#type' => 'select',
    '#title' => '',
    '#options' => $org_opts,
    '#required' => TRUE,
    '#ajax' => array(
      'event' => 'change',
      'callback' => 'ajax_tripal_srna_abundant_project_callback',
      'method'   => 'replace',
      'wrapper'  => 'project-id',
      'effect'   => 'fade',
    ),
  );

  $form['project'] = array(
    '#type' => 'fieldset',
    '#title' => t('Select Project and sample'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#description' => t(''),
  );

  $form['project']['project_id'] = array(
    '#type' => 'select',
    '#title' => 'Project',
    '#options' => array(),
    '#required' => TRUE,
    '#ajax' => array(
      'event' => 'change',
      'callback' => 'ajax_tripal_srna_abundant_sample_callback',
      'method'   => 'replace',
      'wrapper'  => 'sample-id',
      'effect'   => 'fade',
    ),
    '#prefix' => '<div id="project-id">',
    '#suffix' => '</div>',
    '#validated' => TRUE,
  );

  $form['project']['sample_id'] = array(
    '#type' => 'select',
    '#title' => t('Sample'),
    '#options' => array(),
    '#required' => TRUE,
    '#prefix' => '<div id="sample-id">',
    '#suffix' => '</div>',
    '#validated' => TRUE,
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
 * Implements hook_validate 
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
 * Implements hook_submit 
 */
function tripal_srna_abundant_form_submit($form, $form_state) {
  $sample_id = $form_state['values']['sample_id'];
  $top = $form_state['values']['top'];

  $sql = "SELECT e.srna_id, f.sequence, f.length, f.annotation, e.number_count FROM {srna_feature_expression} e 
     LEFT JOIN {srna_feature} f ON e.srna_id = f.srna_id 
     WHERE e.experiment_id = :sample_id 
     ORDER BY e.number_count DESC
     LIMIT :top;
  ";
  $result = db_query($sql, array(':sample_id' => $sample_id, ':top'=>$top));
  $data = array();
  foreach ($result as $record) {
    $array = (array) $record;
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

/**
 * ajax: return project according to select organism
 */
function ajax_tripal_srna_abundant_project_callback($form, &$form_state) {

  $organism_id = $form_state['values']['organism_id'];

  // if (!empty($_SESSION['srna_abundant_select'][$organism_id])) {
  //
  // } 
  // else {
    // get cvterm of miRNA-seq
    $values = array(
      'name' => 'miRNA-Seq',
      'is_obsolete' => 0,
      'cv_id' => array (
       'name' => 'experiment_strategy',
      ),
    );
    $result = chado_select_record('cvterm', array('cvterm_id', 'name'), $values);
    if (empty($result)) {
      drupal_set_message("tripal_srna: can not find type_od of miRNA-Seq", 'error');
    }
    $type_id = $result[0]->cvterm_id;

    // get small RNA experiment
    $sql = "SELECT E.experiment_id, E.name, T.project_id, T.name as project_name, S.biomaterial_id
      FROM chado.experiment E
      LEFT JOIN chado.biomaterial S ON E.biomaterial_id = S.biomaterial_id
      LEFT JOIN chado.project T ON E.project_id = T.project_id
      INNER JOIN chado.experimentprop P ON E.experiment_id = P.experiment_id
      WHERE P.type_id = :type_id AND S.taxon_id = :organism_id 
    ";
    $args = array(':type_id' => $type_id, ':organism_id' => $organism_id);
    $result = db_query($sql, $args);

    // save result to array
    $project_opts = array( '' => '-Select-');
    $sample_opts = array();
    foreach ($result as $exp) {
      $name = $exp->name;
      $project_opts[$exp->project_id] = $exp->project_name;
      if (!$sample_opts[$exp->project_id]) { 
        $sample_opts[$exp->project_id] = array();
      }
      $sample_opts[$exp->project_id][$exp->experiment_id] = $name;
    }

  //$form_state['values']['sample_opts'] = $sample_opts;

  $form['project']['project_id']['#options'] = $project_opts;
  $form['project']['project_id']['#validated'] = TRUE;

  return $form['project']['project_id'];
}

/**
 * ajax: return sample according to selected organism and project
 */
function ajax_tripal_srna_abundant_sample_callback($form, $form_state) {

  $organism_id = $form_state['values']['organism_id'];
  $project_id  = $form_state['values']['project_id'];

    $values = array(
      'name' => 'miRNA-Seq',
      'is_obsolete' => 0,
      'cv_id' => array (
       'name' => 'experiment_strategy',
      ),
    );
    $result = chado_select_record('cvterm', array('cvterm_id', 'name'), $values);
    if (empty($result)) {
      drupal_set_message("tripal_srna: can not find type_od of miRNA-Seq", 'error');
    }
    $type_id = $result[0]->cvterm_id;

    // get small RNA experiment
    $sql = "SELECT E.experiment_id, E.name, T.project_id, T.name as project_name, S.biomaterial_id
      FROM chado.experiment E
      LEFT JOIN chado.biomaterial S ON E.biomaterial_id = S.biomaterial_id
      LEFT JOIN chado.project T ON E.project_id = T.project_id
      INNER JOIN chado.experimentprop P ON E.experiment_id = P.experiment_id
      WHERE P.type_id = :type_id AND S.taxon_id = :organism_id 
    ";
    $args = array(':type_id' => $type_id, ':organism_id' => $organism_id);
    $result = db_query($sql, $args);

    // save result to array
    $sample_opts = array();
    foreach ($result as $exp) {
      $name = $exp->name;
      if ($exp->project_id == $project_id) {
        $sample_opts[$exp->experiment_id] = $name;
      }
    }

  $form['project']['sample_id']['#options'] = $sample_opts;
  $form['project']['sample_id']['#validated'] = TRUE;

  return $form['project']['sample_id'];
}

