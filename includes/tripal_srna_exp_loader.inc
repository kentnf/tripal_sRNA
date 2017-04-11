<?php
/**
 * load sRNA into database
 */

/**
 * 
 */
function tripal_srna_exp_loader_form($form, &$form_state) {

  $form['description'] = array(
    '#markup' => t('This page is for loading small RNA expression into public database (not chado). <br>
       <br>
       <div id=code>Example of of small RNA expression file: <br>
       sRNA ID  sample ID  sample ID<br>
       <br> </div>
    ')
  );

  $form['srna_exp'] = array(
    '#type' => 'textfield',
    '#title' => t('Full path of sRNA expression file'),
    '#maxlength' => 80,
    '#required' => TRUE,
    '#description' => t('example: /var/www/html/icugi3rd/sites/default/files/sRNA_exp.txt'),
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
function tripal_srna_exp_loader_form_validate($form, &$form_state) {

  if (empty($form_state['values']['srna_exp'])) {
    form_set_error('srna_fasta', t('No small RNA exp file input.'));
  } else {
    if (!file_exists($form_state['values']['srna_exp'])) {
      form_set_error('srna_fasta', t('small RNA expression file does not exist.'));
    }
  }
}

/**
 * submit tripal job 
 */
function tripal_srna_exp_loader_form_submit($form, &$form_state) {

  global $user;
  $srna_exp  = $form_state['values']['srna_exp'];

  // submit tripal job for loading small RNA sequence to public database
  $job_args = array(
      $srna_exp
  );

  $job_id = tripal_add_job(
    t('Load sRNA expression: @srna_exp', array('@srna_exp' => $srna_exp)),
    'tripal_srna',
    'tripal_srna_exp_loader_job',
    $job_args,
    $user->uid
  );
}

/**
 * tripal job for loading sRNA expression
 */
function tripal_srna_exp_loader_job($srna_exp) {

  $transaction = db_transaction();
  print "\nNOTE: Loading of small RNA sequence file is performed using a database transaction. \n" .
     "If the load fails or is terminated prematurely then the entire set of \n" .
     "insertions/updates is rolled back and will not be found in the database\n\n";

  try {
    if (!is_readable($srna_exp)) {
      watchdog('tripal_srna_exp_loader',
        'Could not open the file :srna_exp. Check that file exists and that permissions are correct .', 
        array(':srna_exp' => $srna_exp), 
        WATCHDOG_ERROR); 
      exit;
    }

    // parse fasta file and load to srna_feature table
    $num_seq = 0;
    $num_insert = 0;
    $num_update = 0;
    $sample = array();

    $fh = fopen($srna_exp, 'r');
    if ($sample = fgetcsv($fh, 0, "\t")) {
      array_shift($sample);
      foreach ($sample as $sid) {
        $rr = db_query("SELECT sample_id FROM {srna_sample} WHERE sample_id = :sid", array(':sid' => $sid))->fetchField();
        if (!$rr) {
          watchdog('tripal_srna_exp_loader', 
            ':sample does not exist, please input sample info.', array(':sample' => $sid), WATCHDOG_ERROR);
          exit;
        }
      }
    }

    while($line = fgetcsv($fh, 0, "\t")) {
      $srna = array_shift($line);
      $exp = array_combine($sample, $line);
      foreach ($exp as $sample_id => $value) {
        $sql = "SELECT express_id FROM {srna_feature_expression} WHERE srna_id = :srna AND sample_id = :sample";
        $rec = db_query($sql, array(':srna' => $srna, ':sample' => $sample_id))->fetchField();
        if ($rec) {
          $update = db_update('srna_feature_expression')->fields(array(
            'number_count'   => $value,
          ))->condition('express_id', $rec)->execute();
          //drupal_set_message(t('sRNA '.$srna.' expression in sample '.$sample_id.' has been updated.'), 'status');
          ++$num_update;
        }
        else {
          $insert = db_insert('srna_feature_expression')->fields(array(
            'srna_id'      => $srna,
            'sample_id'    => $sample_id,
            'number_count' => $value,
          ))->execute();
          //drupal_set_message(t('sRNA '.$srna.' expression in sample '.$sample_id.' has been inserted.'), 'status');
          ++$num_insert;
        }
        ++$num_seq;
        print  "Insert sRNA exp: " . $num_insert . ", Update sRNA exp: " . $num_update .
          ", sRNA exp " . $num_seq . ". Memory: " .
          number_format(memory_get_usage()) . " bytes.\r";
      }
    }
    fclose($fh);
  }
  catch (Exception $e) {
    print "\n";
    watchdog_exception('tripal_srna_exp_loader', $e);
    $transaction->rollback();
    print "FAILED: Rolling back database changes...\n";
  }
  print "\nDone.\n";
  return;
}

