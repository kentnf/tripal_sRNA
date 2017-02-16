<?php

/**
 * @file
 *
 */

/**
 * generate form for miRNA target prediction.
 */
function mirtarget_pred_form($form, $form_state) {

  // choose mipredseq dataset
  //   An array where the nid is the key and the value is array
  //   array: 
  //      0 the human-readable name of the option
  //      1 path
  //      2 type
  //      3 dblink
  //      4 example 
  $mipredseq = get_mipred_db_options();

  // re-construct the data struc
  // level 1: two array, miRNA and mRNA
  // level 2: array of databases
  $data = array(
    'mRNA' => array(),
    'miRNA' => array(),
  );

  $dblink = array();
 
  foreach ($mipredseq as $nid => $db_array) {
    if ($db_array[2] == 'mRNA') {
      $data['mRNA'][] = $db_array;
    }
    elseif ( $db_array[2] == 'miRNA') {
      $data['miRNA'][] = $db_array;
    }
    $dblink[$db_array[1]] = $db_array[3];
  } 

  // pass the data through form
  $form['#data']  = $data;
  $form['#dblink'] = $dblink;

  $form['SELECT_TYPE'] = array(
    '#type' => 'select',
    '#title' => t('Input query type:'),
    '#options' => array(
      'select_type' => 'select query type',
      'miRNA' => 'microRNA',
      'mRNA'=> 'mRNA',
    ),
    '#default_value' => isset($form['SELECT_TYPE']) ? $form['SELECT_TYPE'] : 'select_type',
    '#ajax' => array(
      'event' => 'change',
      'callback' => 'ajax_mitarget_pred_database_callback',
      'method'   => 'replace',
      'wrapper'  => 'mirtarget-select-db',
      'effect'   => 'fade',
    ),
  );

  $form['SELECT_DATABASE'] = array(
    '#type' => 'select',
    '#title' => t('Choose the database:'),
    '#options' => array(),
    '#validated' => TRUE,
    '#prefix' => '<div id="mirtarget-select-db">',
    '#suffix' => '</div>',  
  );

  $form['INPUT_SEQ'] = array(
    '#type' => 'textarea',
    '#title' => t('Input sequence in fasta format (10 sequences maximum):'),
    '#default_value' => ">test1\nAGAAUCUUGAUGAUGCUGCAU\n>test2\nUCGGACCAGGCUUCAUUCCUC",
  );

  $form['SCORE'] = array(
    '#type' => 'textfield',
    '#title' => t('Score: (0 ~ 4)'),
    '#default_value' => '3',
    '#maxlength' => 20,
  );

  $form['MISMATCH'] = array(
    '#type' => 'textfield',
    '#title' => t('Mismatch: (0 ~ 4)'),
    '#default_value' => '3',
    '#maxlength' => 20,
  );

  $form['GU'] = array(
    '#type' => 'textfield',
    '#title' => t('G:U Wobble: (0 ~ 5)'),
    '#default_value' => '2',
    '#maxlength' => 20,
  );

  $form['INDEL'] = array(
    '#type' => 'textfield',
    '#title' => t('Indel: (0 ~ 1)'),
    '#default_value' => '0',
    '#maxlength' => 20,
  );

  $form['STRAND'] = array(
    '#type' => 'select',
    '#title' => t('Strand:'),
    '#default_value' => 'Both',
    '#options' => array(
       'both' => 'Both',
       'forward' => 'Forward',
       'reverse' => 'Reverse',
     ),
  );

  // Submit
  //.........................
  $form['submit'] = array(
    '#type' => 'submit',
    '#default_value' => ' Submit ',
  );

  $form['#prefix'] = '<div class="row"> <div class="col-md-8 col-md-offset-2">';
  $form['#suffix'] = '</div> </div>';

  return $form;
}

/**
 * Validate the user options submitted via the above form.
 *
 * @see mirtarget_pred_form().
 *
 */
function mirtarget_pred_form_validate($form, &$form_state) {

  $score    = $form_state['values']['SCORE'];
  $mismatch = $form_state['values']['MISMATCH'];
  $gu       = $form_state['values']['GU'];
  $indel    = $form_state['values']['INDEL'];

  // Validate input fasta format

  // check the input sequence before analysis
  // the miRNA should be less than 30bp and the mRNA should be longer than 30bp
  //if ( (length($seqb[1]) <= 30) and ($seq_type eq 1) ) { print "\nPlease check the query sequence type!";  exit; }
  //if ( (length($seqb[1]) > 30) and ($seq_type eq 0) ) { print "\nPlease check the query sequence type!";  exit; }

  // this is just a plan for validate fasta
  if (!empty($form_state['values']['INPUT_SEQ'])) {
    if (validate_fasta_sequence($form_state['values']['INPUT_SEQ'])){
      form_set_error('INPUT_SEQ', t('You need to provide a valid FASTA sequence '
      . 'for the query. It should contain a FASTA header/definition line '
      . 'followed by %molecule-type. For more information see the '
      . '<a href="@url" target="_blank">NCBI FASTA Specification</a>.',
        array(
          '%molecule-type' => $molecule_type,
          '@url' => 'http://www.ncbi.nlm.nih.gov/BLAST/blastcgihelp.shtml'
      )));
    } 
  } else {
    form_set_error('INPUT_SEQ', t('No sequence file.'));
  }

  // Validate Parameters
  if (!is_numeric($score) or $score > 4 or $score < 0) {
    form_set_error('SCORE', t('Score is out of range (0 ~ 4).'));
  }

  if (!is_numeric($mismatch) or $mismatch > 4 or $mismatch < 0) {
    form_set_error('MISMATCH', t('Mismatch is out of range (0 ~ 4).'));
  }

  if (!is_numeric($gu) or $gu > 5 or $gu < 0) {
    form_set_error('GU', t('G:U Wobble is out of range (0 ~ 5).'));
  }

  if (!is_numeric($indel) or $indel > 1 or $indel < 0) {
    form_set_error('INDEL', t('Indel is out of range (0 ~ 1).'));
  }
}

/**
 * Process the user options submitted.
 *
 * @see mirtarget_pred_form().
 */
function mirtarget_pred_form_submit($form, &$form_state) {

  $error = FALSE;

  $qtype    = $form_state['values']['SELECT_TYPE'];
  $database = $form_state['values']['SELECT_DATABASE'];
  $qseq     = $form_state['values']['INPUT_SEQ'];
  $score    = $form_state['values']['SCORE'];
  $mismatch = $form_state['values']['MISMATCH'];
  $gu       = $form_state['values']['GU'];
  $indel    = $form_state['values']['INDEL'];
  $strand   = $form_state['values']['STRAND'];
  // query type
  $qtype_param = 0;
  if ($qtype == 'mRNA') {
    $qtype_param = 1;
  }

  // save the query seq to temp file
  $qseq_file = '/tmp/' . date('YMd_His') . '_miRPred.fasta';
  file_put_contents($qseq_file, $qseq);

  // check if database is readable
  if (!is_readable($database)) {
    $error = TRUE;
    // Reports error in admin>>reports>>Recent Log Messages
    tripal_report_error(
      'miRNA target prediction',
      TRIPAL_ERROR,
      "The database for miRNA target prediction is unaccessible."
    );
    // report error in current page
    $msg = "The database '$database' is unaccessible. ";
    $msg .= "Please contact the site administrator.";
    drupal_set_message($msg, 'error');
  }

  // OPTIONS
  $options = array();
  if ( isset($form['#dblink'][$database]) ) {
    $options = array(
      'q' => $qtype_param,
      'x' => $form['#dblink'][$database],
      's' => $score,
      'm' => $mismatch,
      'w' => $gu,
      'i' => $indel,
      'r' => $strand,
    );

  } else {
    $error = TRUE;
    tripal_report_error(
      'miRNA target prediction',
      TRIPAL_ERROR,
      "The sequence ID linkout is undetermined."
    );
  }

  dpm($form);
  dpm($form_state);

  // SUBMIT JOB TO TRIPAL
  //-------------------------
  if (!$error) {
    global $user;

    // set output dir and file
    $output_dir = variable_get('file_public_path', conf_path() . '/files')
      . DIRECTORY_SEPARATOR . 'tripal' . DIRECTORY_SEPARATOR . 'tripal_mirtarget';
    $output_file = $output_dir . DIRECTORY_SEPARATOR . date('YMd_His') . '.miRTargetPred.txt';

    $job_args = array(
      'program'  => 'miRNA_target_pred.pl',
      'query'    => $qseq_file,
      'database' => $database,
      'output'   => $output_file,
      'options'  => $options
    );

    $job_id = tripal_add_job(
      t('miRNA target prediction @db', array('@db' => $database)),
      'tripal_miRNA',
      'run_mirtarget_tripal_job',
      $job_args,
      $user->uid
    );

	dpm($job_args, 'jobargs');

    // Encode the job_id
    $job_encode_id = mirtarget_make_secret($job_id);

	dpm($job_encode_id, 'job_id');

    // Redirect to the miRNA target prediction result page
    // drupal_goto("mirtarget/report/$job_encode_id");
  }
}

/**
 * AJAX: Replace the ID textarea with one containing an example.
 */
function ajax_mitarget_pred_database_callback($form, $form_state) {
  $type = $form_state['values']['SELECT_TYPE'];
  $data = $form['#data'];

  // determine db according the type
  $db = 'NA';
  if ($type == 'miRNA') {
    $db = $data['mRNA'];
  } else {
    $db = $data['miRNA'];
  }

  $db_options = array(
    'select_db' => 'select a database'
  );

  // choose mipredseq dataset
  //   An array where the nid is the key and the value is array
  //   array: 
  //      0 the human-readable name of the option
  //      1 path
  //      2 type
  //      3 dblink
  //      4 example 
  foreach ($db as $db_array) {
    $db_options[$db_array[1]] = $db_array[0];
  }

  $form['SELECT_DATABASE']['#options'] = $db_options;
  $form['SELECT_DATABASE']['#validated'] = TRUE;
  return $form['SELECT_DATABASE'];
}

