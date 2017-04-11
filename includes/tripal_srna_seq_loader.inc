<?php
/**
 * load sRNA into database
 */

/**
 * 
 */
function tripal_srna_seq_loader_form($form, &$form_state) {

  $form['description'] = array(
    '#markup' => t('This page is for loading small RNA sequence (fasta) into public database (not chado). <br>
       <br>
       <div id=code>Example of of small RNA format: <br>
       >P_S0000704 miRNA <br>
       TCACAGTGAACCGGTCTCTTTT<br> </div>
       <p>* the miRNA behind sequence id is the annotation of small RNA sequence, it could be miRNA, tRNA, rRNA, or etc.</p>
    ')
  );

  $org_opts = tripal_srna_get_organism_options();
  $form['organism_id'] = array(
    '#type' => 'select',
    '#title' => 'Select Genome',
    '#options' => $org_opts,
    '#required' => TRUE,
  );

  $form['srna_fasta'] = array(
    '#type' => 'textfield',
    '#title' => t('Full path of sRNA fasta'),
    '#maxlength' => 50,
    '#required' => TRUE,
    '#description' => t('example: /var/www/html/icugi3rd/sites/default/files/sRNA.fasta'),
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
function tripal_srna_seq_loader_form_validate($form, &$form_state) {

  if (empty($form_state['values']['organism_id'])) {
    form_set_error('organism_id', t('No genome selected. Choose a genome from the list.'));
  }
  if (empty($form_state['values']['srna_fasta'])) {
    form_set_error('srna_fasta', t('No small RNA fasta file input.'));
  } else {
    if (!file_exists($form_state['values']['srna_fasta'])) {
      form_set_error('srna_fasta', t('small RNA fasta file does not exist.'));
    }
  }
}

/**
 * submit tripal job 
 */
function tripal_srna_seq_loader_form_submit($form, &$form_state) {

  global $user;
  $organism_id = $form_state['values']['organism_id'];
  $srna_fasta  = $form_state['values']['srna_fasta'];

  // submit tripal job for loading small RNA sequence to public database
  $job_args = array(
      $organism_id, $srna_fasta
  );

  $job_id = tripal_add_job(
    t('Load sRNA sequence: @srna_fasta', array('@srna_fasta' => $srna_fasta)),
    'tripal_srna',
    'tripal_srna_seq_loader_job',
    $job_args,
    $user->uid
  );
}

/**
 * tripal job for loading small RNA sequences
 */
function tripal_srna_seq_loader_job($organism_id, $srna_fasta) {

  $transaction = db_transaction();
  print "\nNOTE: Loading of small RNA sequence file is performed using a database transaction. \n" .
     "If the load fails or is terminated prematurely then the entire set of \n" .
     "insertions/updates is rolled back and will not be found in the database\n\n";

  try {
    if (!is_readable($srna_fasta)) {
      exit("Could not open the file '$srna_fasta'.  Check that file exists and that permissions are correct.");
    }

    // parse fasta file and load to srna_feature table
    $num_seq = 0;
    $num_insert = 0;
    $num_update = 0;

    $fh = fopen($srna_fasta, 'r');
    while($line = fgets($fh)) {
      $id = explode(' ', $line);
      $sid = trim(preg_replace('/^>/', '', $id[0]));
      $seq = trim(fgets($fh));
      $annotation = trim($id[1]);
      $len = strlen($seq);

      if (db_query("SELECT 1 FROM {srna_feature} WHERE srna_id = :sid", array(':sid' => $sid))->fetchField()) {
        $update = db_update('srna_feature')->fields(array(
          'sequence'    => $seq,
          'length'      => $len,
          'annotation'  => $annotation,
          'organism_id' => $organism_id,
        ))->condition('srna_id', $sid)->execute();
        //drupal_set_message(t('sRNA '.$sid.' has been updated.'), 'status');
        ++$num_update;
      }
      else {
        $insert = db_insert('srna_feature')->fields(array(
          'srna_id'     => $sid,
          'sequence'    => $seq,
          'length'      => $len,
          'annotation'  => $annotation,
          'organism_id' => $organism_id,
        ))->execute();
        //drupal_set_message(t('sRNA '.$sid.' has been insert into database.'), 'status');
        ++$num_insert;
      }
      ++$num_seq;
      print  "Insert sRNA: " . $num_insert . ", Update sRNA: " . $num_update .
          ", sRNA seqs " . $num_seq . ". Memory: " .
          number_format(memory_get_usage()) . " bytes.\r";
    }
    fclose($fh);
  }
  catch (Exception $e) {
    print "\n";
    watchdog_exception('tripal_srna_seq_loader', $e);
    $transaction->rollback();
    print "FAILED: Rolling back database changes...\n";
  }
  print "\nDone.\n";
  return;
}

