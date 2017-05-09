<?php
/**
 * view sRNA using JBrowse
 *
 */

/**
 * 
 */
function tripal_srna_view_form($form, $form_state) {

  // drupal_set_title(t('New title for the node form'));

  // search mapped siRNA by genome, chr, start, end
  $first_opt = array('SELECT_G' => '- select -');
  $org_opts = tripal_srna_get_organism_options();
  // $form['#org_opts']  = $org_opts; // pass org array through from
  $org_opts = $first_opt + $org_opts;

  $form['LOCATION'] = array(
    '#type' => 'fieldset',
    '#title' => t('Step 1: Search siRNA by location.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );


  $form['LOCATION']['SELECT_GENOME'] = array(
    '#type' => 'select',
    '#title' => t('Genome:'),
    '#options' => $org_opts,
    '#default_value' => 'SELECT_G', // put default db for different web
    '#ajax' => array(
      'event' => 'change',
      'callback' => 'ajax_srnaview_location_callback',
      'method'   => 'replace',
      'wrapper'  => 'id-textarea',
      'effect'   => 'fade',
    ),
  );

  $second_opt = array('SELECT_C' => '- select -');

  $form['LOCATION']['SELECT_CHR'] = array(
    '#type' => 'select',
    '#title' => t('Chromosome/Scaffold:'),
    '#prefix' => '<div id="id-textarea">',
    '#suffix' => '</div>',
    '#options' => $second_opt,
    '#default_value' => 'SELECT_C', // put default db for different web
    '#validated' => TRUE,

    //'#ajax' => array(
      //'event' => 'change',
      // 'callback' => 'ajax_synview_location_callback',
      //'method'   => 'replace',
      //'wrapper'  => 'id-textarea',
      //'effect'   => 'fade',
      //),
  );

  $form['LOCATION']['START'] = array(
    '#type' => 'textfield',
    '#title' => t('Start:'),
    '#maxlength' => 20,
    '#prefix' =>'<div id=xyz>',
  );

  $form['LOCATION']['END'] = array(
    '#type' => 'textfield',
    '#title' => t('End:'),
    '#maxlength' => 20,
    '#suffix' =>'</div>',
  );

  //.........................
  $form['ID'] = array(
    '#type' => 'fieldset',
    '#title' => t('<b>OR</b> Search siRNA by gene ID.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  $form['ID']['INPUT_ID'] = array(
    '#type' => 'textfield',
    '#title' => t('Enter gene ID. Example: orange1.1g015632m '),
    '#maxlength' => 20,
  );


  $form['sample'] = array(
    '#type' => 'fieldset',
    '#title' => t('Step2: Input sRNA sample name (Experiment Name).'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  $form['sample']['list'] = array(
    '#type' => 'textarea',
    '#title' => t('one experiment name per line.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  // Submit
  //.........................
  $form['submit'] = array(
    '#type' => 'submit',
    '#default_value' => ' Search ',
  );

  $form['#prefix'] = '<div class="row"> <div class="col-md-8 col-md-offset-2">';
  $form['#suffix'] = '</div> </div>';

  return $form;
}

/**
 * 
 */
function tripal_srna_view_form_validate($form, &$form_state) {

  //checi if the samples in list are small RNA samples
  $exp_org = array();

  if ($form_state['values']['list']) {
    $lists = explode(PHP_EOL, trim($form_state['values']['list']));
    $sql = "SELECT E.experiment_id, S.taxon_id FROM chado.experiment E
      LEFT JOIN chado.biomaterial S ON E.biomaterial_id = S.biomaterial_id
      LEFT JOIN chado.experimentprop P ON E.experiment_id = P.experiment_id
      LEFT JOIN chado.cvterm C ON P.type_id = C.cvterm_id
      WHERE 
        E.name = :exp_name AND
        C.name = :mirna
     ";

    $new_lists = array();

    foreach ($lists as $exp_name) {
      $exp_name = trim($exp_name);
      $r = db_query($sql, array(':exp_name' => $exp_name, ':mirna' => 'miRNA-Seq'))->fetchAll();
      if (sizeof($r) == 1) {
        $taxon_id = $r[0]->taxon_id;
        $new_lists[] = $exp_name;
        $exp_org[$exp_name] = $taxon_id;
      }
      else {
        form_set_error('list', t(':exp_name is not sRNA experiment.', array(':exp_name'=>$exp_name)));
      }
    }
    $form_state['values']['lists'] = $new_lists;
  }

  if (empty($form_state['values']['INPUT_ID'])) {
    // if no block ID is provided
    // the genome, chr, and ref genome should be required
    $org_id = $form_state['values']['SELECT_GENOME'];
    $chr    = $form_state['values']['SELECT_CHR'];
    $start  = $form_state['values']['START'];
    $end    = $form_state['values']['END'];

    if (empty($form_state['values']['SELECT_GENOME']) || $form_state['values']['SELECT_GENOME'] == 'SELECT_G') {
      form_set_error('SELECT_GENOME', t('No genome selected. Choose a genome from the list.'));
    }

    if (empty($chr)) {
      form_set_error('SELECT_CHR', t('No chromosome selected. Choose a chromosome from the list.'));
    }

    if ($start and !preg_match('/^\d+$/', $start)) {
      form_set_error('START', 'Please enter a positive numeric value for the start');
    }
    if ($end and !preg_match('/^\d+$/', $end)) {
      form_set_error('END', 'Please enter a positive numeric value for the end');
    }
    $form_state['byFlag'] = 'location';
    $form_state['values']['organism_id'] = $org_id;

  } else {
    $result = chado_select_record(
      'feature',
      array('uniquename', 'organism_id'), 
      array('uniquename' => $form_state['values']['INPUT_ID'])
    ); 

    if (sizeof($result) != 1) {
      form_set_error('INPUT_ID', 'Please enter a valid feature name');
    } 
    $form_state['byFlag'] = 'id';
    $org_id = $result[0]->organism_id;
    $form_state['values']['organism_id'] = $org_id;
  }

  // check if the list of experiments match to the organism 
  foreach ($exp_org as $exp_name => $organism_id) {
    if ($org_id != $organism_id) {
      form_set_error('list', t(':exp_name does not belong to the selected organism.', array(':exp_name'=>$exp_name)));
    }
  }
}

/**
 *  
 */
function tripal_srna_view_form_submit($form, $form_state) {

  // save sample (experiment) lists to session
  $exp_lists = array();
  if (isset($form_state['values']['lists'])) {
    //$_SESSION['tripal_sirnaview_search']['LISTS'] = $form_state['values']['lists'];
    $exp_lists = $form_state['values']['lists'];
  } else {
    // select all small RNA samples according to organism id
    $sql = "SELECT E.experiment_id, E.name FROM chado.experiment E
      LEFT JOIN chado.biomaterial S ON E.biomaterial_id = S.biomaterial_id
      LEFT JOIN chado.experimentprop P ON E.experiment_id = P.experiment_id
      LEFT JOIN chado.cvterm C ON P.type_id = C.cvterm_id
      WHERE 
        S.taxon_id = :organism_id AND
        C.name = :mirna
      ";
    $results = db_query($sql, array(':organism_id' => $form_state['values']['organism_id'], ':mirna' => 'miRNA-Seq'))->fetchAll();
    foreach ($results as $r) {
      $exp_lists[] = $r->name;
    }
  }

  // determine jbrowse_data and jbrowse_tracks
  $jb_tracks = array();
  foreach ($exp_lists as $exp_name) {
    $experiment = chado_generate_var('experiment', array('name' => $exp_name));
    $experiment = chado_expand_var($experiment, 'table', 'experimentprop', array('return_array' => 1));
    foreach ($experiment->experimentprop as $prop) {
      if ($prop->type_id->name == 'jbrowse_data') {
        $jb_data = $prop->value;
      }
      if ($prop->type_id->name == 'jbrowse_track') {
        $jb_tracks[] = $prop->value;
      }
    }
  }

  $_SESSION['tripal_sirnaview_search']['data'] = $jb_data;
  $_SESSION['tripal_sirnaview_search']['tracks'] = $jb_tracks;

  // save search by location to session
  if ($form_state['byFlag'] == 'location') {
    $org_id = $form_state['values']['SELECT_GENOME'];
    $chr = $form_state['values']['SELECT_CHR'];
    $start = $form_state['values']['START'];
    $end = $form_state['values']['END'];
    $_SESSION['tripal_sirnaview_search']['location'] = "$chr:$start..$end";
    $_SESSION['tripal_sirnaview_search']['METHOD']   = 'location';
    drupal_goto("srna/view/display");
  }
  // save search by id to session
  elseif ($form_state['byFlag'] == 'id') {
    $_SESSION['tripal_sirnaview_search']['location'] = $form_state['values']['INPUT_ID'];
    $_SESSION['tripal_sirnaview_search']['METHOD']   = 'id';
    drupal_goto("srna/view/display");
  }
  else {
    drupal_set_message("Can not determine the location of siRNA view.", 'warning');
    tripal_report_error(
      'tripal_srna',
      TRIPAL_ERROR,
      "Can not determine the location of siRNA view."
    );
  }
}

/**
 * display sRNA view result 
 */
function tripal_srna_view_display() {
  // return search page; 
  if (empty($_SESSION['tripal_sirnaview_search']['METHOD'])) {
    drupal_goto("srna/view");
  } else {
    return theme('srna_view_display');
  }
}

/**
 * AJAX: change the selection of chr according genome.
 */
function ajax_srnaview_location_callback($form, $form_state) {

  $genome_sel = $form_state['values']['SELECT_GENOME'];

  $sql = "SELECT F.feature_id, F.uniquename FROM chado.feature F
     LEFT JOIN chado.cvterm C ON F.type_id = C.cvterm_id
     WHERE
       F.organism_id = :organism_id AND
       C.name = :name1
     ORDER BY 
       F.uniquename";
  $param = array(
    ':organism_id' => $genome_sel,
    ':name1' => 'chromosome',
    //':name2' => 'supercontig',
  );
  $results = chado_query($sql, $param);

  $chr_opts = $form['LOCATION']['SELECT_CHR']['#options'];
  foreach ($results as $r) {
    $chr_opts[$r->uniquename] = $r->uniquename;
  }

  $form['LOCATION']['SELECT_CHR']['#options'] = $chr_opts;
  $form['LOCATION']['SELECT_CHR']['#validated'] = TRUE;

  return $form['LOCATION']['SELECT_CHR'];
}




