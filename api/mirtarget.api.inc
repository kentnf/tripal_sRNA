<?php

/**
 * @file
 * Contains more generally applicable functions as well as some meant to help developers
 * Plug-in to the BLAST UI functionality
 */

/**
 * Get a specific BlastDB.
 *
 * @param $identifiers
 *   An array of identifiers used to determine which BLAST DB to retrieve.
 *
 * @return
 *   A fully-loaded BLAST DB Node
 */
/**
function get_blast_database($identifiers) {
  $node = FALSE;

  if (isset($identifiers['nid'])) {
    $node = node_load($identifiers['nid']);
  }
  elseif (isset($identifiers['name'])) {

    $nid = db_query('SELECT nid FROM {blastdb} WHERE name=:name', array(':name' => $identifiers['name']))->fetchField();
    $node = node_load($nid);

  } elseif (isset($identifiers['path'])) {

    $nid = db_query('SELECT nid FROM {blastdb} WHERE path LIKE :path', array(':path' => db_like($identifiers['path']) . '%'))->fetchField();
    $node = node_load($nid);

  }

  return $node;
}
 */

/**
 * Returns a list mipredseq DB options
 *
 * @return
 *   An array where the nid is the key and the value is array
 *   array: 
 *      1 the human-readable name of the option
 *      2 path
 *      3 type
 *      4 link for sequence id
 *      5 example sequence
 */
function get_mipred_db_options() {
  global $user;

  // Use the Entity API to get a list of pwy Nodes to load
  // We use this function in order respect node access control so that
  // administrators can use this module in combination with a node access module
  // of their choice to limit access to specific BLAST databases.
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    // Restrict to pwy nodes.
    ->entityCondition('bundle', 'mipredseq')
    // Restrict to Published nodes.
    ->propertyCondition('status', 1)
    // Restrict to nodes the current user has permission to view.
    ->addTag('node_access');
  $entities = $query->execute();
  // Get all pwy nodes
  $nodes  = node_load_multiple(array_keys($entities['node']));

  $options = array();
  foreach ($nodes as $node) {
    if ( isset($node) && isset($node->db_type) && isset($node->db_name) && isset($node->db_path) && isset($node->db_dblink) ) {
      $options[$node->nid] = array(
        $node->db_name,
        $node->db_path,
        $node->db_type,
        $node->db_dblink,
        $node->db_example_seq,
      );
    }
  }
  asort($options);

  return $options;
}

/** 
 * Run pwy enrichment job ()
 *
 */
function run_mirtarget_tripal_job($program, $gene_list, $pwy_file, $output_file, $options, $job_id = NULL) {

  print "\nExecuting $program\n\n";
  print "Gene List: $gene_list\n";
  print "PWY File: $pwy_file\n";
  print "Results File: $output_file\n";

  // Note: all variables are escaped (adds single quotes around their values) for security reasons.
  $pwyenrich_cmd = escapeshellarg($program);
  if (!empty($options)) {
    foreach ($options as $opt => $val) {
      $val = trim($val);
      if (!empty($val)) {
        print "\t$opt: $val\n";
        // We want to escape all the option values since they were supplied via
        // user input. These values should also have been checked in the
        // advanced form _validate functions but this adds an extra layer of
        // protection.
        $pwyenrich_cmd .= ' -' . escapeshellarg($opt) . ' ' . escapeshellarg($val);
      }
    }
  }
  //$pwyenrich_cmd.= ' -r ' . DRUPAL_ROOT;
  $pwyenrich_cmd.= ' '. escapeshellarg($gene_list) . ' ' . escapeshellarg($pwy_file) . ' ' . escapeshellarg($output_file);

  // Setting the value of threads by admin page
  print "\nExecuting the following pwy enrichment command:\n" . $pwyenrich_cmd . "\n";

  system($pwyenrich_cmd); # just print command , do not run it 

  if (!file_exists($output_file)) {
    tripal_report_error(
      'pwy_tool',
      TRIPAL_ERROR,
      "pathway enrichment did not complete successfully as is implied by the lack of output file (%file). The command run was @command",
      array('%file' => $output_file, '@command' => $pwyenrich_cmd),
      array('print' => TRUE)
    );
    return FALSE;
  }

  print "done";
}

/**
 * FASTA validating parser
 *
 * A sequence in FASTA format begins with a single-line description, followed
 * by lines of sequence data.The description line is distinguished from the
 * sequence data by a greater-than (">") symbol in the first column. The word
 * following the ">" symbol is the identifier of the sequence, and the rest of
 * the line is the description (both are optional). There should be no space
 * between the ">" and the first letter of the identifier. The sequence ends
 * if another line starting with a ">" appears which indicates the start of
 * another sequence.
 *
 * @param $sequence
 *  A string of characters to be validated.
 *
 * @return
 *  Return a boolean. 1 if the sequence does not pass the format valifation stage and 0 otherwise.
 *
 */
function validate_fasta_sequence($sequence) {

  //  http://www.cnblogs.com/xudongliang/p/5028704.html

  $fastaIdRegEx = '/^>.*(\\n|\\r)/';
  $fastaSeqRegEx = '/[^ATCGNUKMBVSWDYRHatcgnukmbvswdyrh\n\r]/'; //Includes IUPAC codes.
  if ( preg_match($fastaSeqRegEx,$sequence) && !(preg_match($fastaIdRegEx,$sequence)) ) {
    return TRUE;
  }
  return FALSE;
}

