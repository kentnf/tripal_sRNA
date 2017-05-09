<?php
/**
 * load mirna into database
 */

/**
 * 
 */
function tripal_mirna_loader_form($form, &$form_state) {


  $head_data = array('miRNA ID*','sRNA ID', 'miRNA family', 'sequence(option)');
  $rows_data = array('P_M00018','P_S0029018', 'miR393', 'UCCAAAGGGAUCGCAUUGAUC');
  $t1 = tripal_srna_table_example($head_data, $rows_data);

  $head_data = array('hairpin ID*','miRNA ID', 'reference', 'start', 'end', 'strand', 
    'miR_start', 'miR_end', 'sequence', 'fold eneryg');
  $rows_data = array('H000006','P_M00018', 'Scaffold000029', '1734514', '1734611', '1', '21', '+', 
    'UCCAAAGGGAUCGCAUUGAUCCAAU...', '-42.60');
  $t2 = tripal_srna_table_example($head_data, $rows_data);

  $head_data = array('miRNA ID*','hit ID*', 'align_query', 'align_string', 'align_hit');
  $rows_data = array('P_M00018','ath-miR393a', 'UCCAAAGGGAUCGCAUUGAUC-', '|||||||||||||||||||||x', 
    'UCCAAAGGGAUCGCAUUGAUCC');
  $t3 = tripal_srna_table_example($head_data, $rows_data);

  $head_data = array('miRNA ID*','target feature uniquename*', 'start', 'end', 'score', 
    'align_query', 'align_hit', 'align_string', 'strand');
  $rows_data = array('P_M00018','MU21869', '778', '798', '2',
    'AAACAAUGCGAUCCCUUUGGA', 'CUAGUUACGCUAGGGAAACCU', 'b|b||||||||||||||||||', '+');
  $t4 = tripal_srna_table_example($head_data, $rows_data);

  $head_data = array('hairpin ID*','sRNA ID*', 'start', 'end', 'sequence(option)');
  $rows_data = array('H000007','P_S0010340', '58', '78', 'TTGGATCATGCTATCCCTTCG');
  $t5 = tripal_srna_table_example($head_data, $rows_data);

  $form['description'] = array(
    '#markup' => t('This page is for loading miRNA table into database. Before loading miRNA
       tables, the small RNA sequence should be loaded. All the table should be 
       tab-delimit format <b>without title</b> before loading to database. Here, we just give 
       an example of each table. <br> If miRNA record already exist in database, it will be 
       updated instead of insert. 
       <br>
       <b>Example of miRNA table:</b> <br>' . $t1 . 
       '<b>Example of pre-miRNA table:</b> <br>' . $t2 .
       '<b>Example of miRNA conserved table:</b> <br>' . $t3 .
       '<b>Example of miRNA target table:</b> <br>' . $t4 . 
       '<b>Example of miRNA star table:</b> <br>' . $t5 . '<br>'
   ));

  $org_opts = tripal_srna_get_organism_options();
  $form['organism_id'] = array(
    '#type' => 'select',
    '#title' => 'Select Genome',
    '#options' => $org_opts,
    '#required' => TRUE,
  );

  $form['mirna_tab'] = array(
    '#type' => 'textfield',
    '#title' => t('Full path of miRNA table'),
    '#maxlength' => 255,
    '#required' => TRUE,
    '#description' => t('example: /var/www/html/icugi3rd/sites/default/files/miRNA.txt'),
  );

  $form['pre_mirna_tab'] = array(
    '#type' => 'textfield',
    '#title' => t('Full path of pre-miRNA table'),
    '#maxlength' => 255,
    '#required' => TRUE,
    '#description' => t('example: /var/www/html/icugi3rd/sites/default/files/miRNA_hairpin.txt'),
  );

  $form['mirna_conserved_tab'] = array(
    '#type' => 'textfield',
    '#title' => t('Full path of miRNA conserved table'),
    '#maxlength' => 255,
    '#required' => TRUE,
    '#description' => t('example: /var/www/html/icugi3rd/sites/default/files/miRNA_conserved.txt'),
  );

  $form['mirna_target_tab'] = array(
    '#type' => 'textfield',
    '#title' => t('Full path of miRNA target table'),
    '#maxlength' => 255,
    '#description' => t('example: /var/www/html/icugi3rd/sites/default/files/miRNA_target.txt'),
  );

  $form['mirna_star_tab'] = array(
    '#type' => 'textfield',
    '#title' => t('Full path of miRNA star table'),
    '#maxlength' => 255,
    '#description' => t('example: /var/www/html/icugi3rd/sites/default/files/miRNA_star.txt'),
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
function tripal_mirna_loader_form_validate($form, $form_state) {

  if (empty($form_state['values']['organism_id'])) {
    form_set_error('organism_id', t('No genome selected. Choose a genome from the list.'));
  }

  $form_names = array('mirna_tab', 'pre_mirna_tab', 'mirna_conserved_tab');
  foreach ($form_names as $form_name) {
    if (empty($form_state['values'][$form_name])) {
      form_set_error($form_name, t('No input table ' . $form_name . "."));
    } else {
      if (!file_exists($form_state['values'][$form_name])) {
        form_set_error($form_name, t('Table file ' . $form_name . ' does not exist.'));
      }
    }
  }

  $form_names = array('mirna_target_tab', 'mirna_star_tab');
  foreach ($form_names as $form_name) {
    if (!empty($form_state['values'][$form_name])) {
      if (!file_exists($form_state['values'][$form_name])) {
        form_set_error($form_name, t('Table file ' . $form_name . ' does not exist.'));
      }
    }
  }
}

/**
 * submit tripal job 
 */
function tripal_mirna_loader_form_submit($form, &$form_state) {

  global $user;
  $organism_id         = $form_state['values']['organism_id'];
  $mirna_tab           = $form_state['values']['mirna_tab'];
  $mirna_hairpin_tab   = $form_state['values']['pre_mirna_tab'];
  $mirna_conserved_tab = $form_state['values']['mirna_conserved_tab'];
  $mirna_target_tab    = $form_state['values']['mirna_target_tab'];
  $mirna_star_tab      = $form_state['values']['mirna_star_tab'];

  // submit tripal job for loading miRNA sequence to public database
  $job_args = array(
    $organism_id, $mirna_tab, $mirna_hairpin_tab, $mirna_conserved_tab, $mirna_target_tab, $mirna_star_tab
  );

  $job_id = tripal_add_job(
    t('Load miRNA table for: @org', array('@org' => $organism_id)),
    'tripal_srna',
    'tripal_mirna_loader_job',
    $job_args,
    $user->uid
  );
}

/**
 * tripal job for loading miRNA and corresponding tables
 */
function tripal_mirna_loader_job($organism_id, $mirna_tab, $mirna_hairpin_tab, $mirna_conserved_tab, $mirna_target_tab, $mirna_star_tab) {

  $transaction = db_transaction();
  print "\nNOTE: Loading of miRNA tables is performed using a database transaction. \n" .
     "If the load fails or is terminated prematurely then the entire set of \n" .
     "insertions/updates is rolled back and will not be found in the database\n\n";

  try {
    // check required files 
    $required_files = array($mirna_tab, $mirna_hairpin_tab, $mirna_conserved_tab);
    foreach ($required_files as $file) {
      if (!is_readable($file)) {
        watchdog('tripal_mirna_loader',
          'Could not open the file :file. Check that file exists and that permissions are correct .', 
          array(':file' => $file), 
          WATCHDOG_ERROR); 
        exit;
      }
    }

    $insert_key = array('mirna_id', 'srna_id', 'family', 'organism_id');
    $where_key = array('mirna_id');
    $unique_key = 'mirna_id';
    insert_update_db($mirna_tab, 'mirna_feature', $insert_key, $where_key, $unique_key);

    $insert_key = array(
      'hairpin_id', 'mirna_id', 'genome_id', 'hit_start', 'hit_end',
      'mir_start', 'mir_end', 'strand', 'sequence', 'folding_energy'
    );
    $where_key = array('hairpin_id');
    $unique_key = 'hairpin_id';
    insert_update_db($mirna_hairpin_tab, 'mirna_hairpin', $insert_key, $where_key, $unique_key);

    $insert_key = array('mirna_id', 'hit_id', 'align_query', 'align_string', 'align_hit');
    $where_key = array('mirna_id', 'hit_id');
    $unique_key = 'cid';
    insert_update_db($mirna_conserved_tab, 'mirna_conserved', $insert_key, $where_key, $unique_key);

    if ($mirna_target_tab) {
      $insert_key = array(
        'mirna_id', 'target_id', 'align_start', 'align_end', 'score',
        'align_query', 'align_string', 'align_hit', 'strand' 
      );
      $where_key  = array('mirna_id', 'target_id');
      $unique_key = 'tid';
      insert_update_db($mirna_target_tab, 'mirna_target', $insert_key, $where_key, $unique_key);
    }

    if ($mirna_star_tab) {
      $insert_key = array('hairpin_id', 'srna_id', 'srna_start', 'srna_end');
      $where_key  = array('hairpin_id', 'srna_id');
      $unique_key = 'mir_star_id';
      insert_update_db($mirna_star_tab, 'mirna_star', $insert_key, $where_key, $unique_key);
    }
  }
  catch (Exception $e) {
    print "\n";
    watchdog_exception('tripal_mirna_loader', $e);
    $transaction->rollback();
    print "FAILED: Rolling back database changes...\n";
  }
  print "\nDone.\n";
  return;
}

/**
 * insert or update database for simple table 
 * (works only if the table file has the same format with table in database) 
 *
 * @param file -- file path and name (tab delimit format) for insert or update
 * 
 * @param table -- table name in sql database 
 * 
 * @param insert_key -- keys's name (column name) of file
 * 
 * @param where_key -- keys for find duplicate record for update
 *
 * @param unique_key -- unique key for table for update record
 */
function insert_update_db ($file, $table, $insert_key, $where_key, $unique_key) {

    print "Load (update) file $file into database ....\n";

    // parse fasta file and load to srna_feature table
    $num_total = 0;
    $num_insert = 0;
    $num_update = 0;

    $fh = fopen($file, 'r');
    while($line = fgetcsv($fh, 0, "\t")) {
      $insert_array = array();
      if (sizeof($insert_key) == sizeof($line)) {
        $insert_array = array_combine($insert_key, $line);
      } else {
        watchdog('tripal_mirna_loader',
          'Insert key number does not match column number in :file.',
          array(':file' => $file),
          WATCHDOG_ERROR
        );
      }

      $where_clause = '';
      $where_value = array();
      $n = 0;
      foreach ($where_key as $wkey) {
        ++$n;
        if ($n > 1) {
          $where_clause.= " AND ";
        }
        $where_clause.= "$wkey = :$wkey";
        $where_value[$wkey] = $insert_array[$wkey];
      }

      $sql = "SELECT $unique_key FROM {$table} WHERE $where_clause";
      $rec = db_query($sql, $where_value)->fetchField();
      if ($rec) {
        $update = db_update($table)->fields($insert_array)->condition($unique_key, $rec)->execute();
        //drupal_set_message(t('sRNA '.$srna.' expression in sample '.$sample_id.' has been updated.'), 'status');
        ++$num_update;
      }
      else {
        /** 
         * insert array format
         * 'srna_id'      => $line[0],
         * 'sample_id'    => $line[1],
         * '...'          -> $line[2],
         */
        $insert = db_insert($table)->fields($insert_array)->execute();
          //drupal_set_message(t('sRNA '.$srna.' expression in sample '.$sample_id.' has been inserted.'), 'status');
        ++$num_insert;
      }
      ++$num_total;
      print "Insert record: " . $num_insert . ", Update record: " . $num_update .
        ", Total record " . $num_total . ". Memory: " .
        number_format(memory_get_usage()) . " bytes.\r";
    }
    fclose($fh);

    print "Done\n"; 

}

