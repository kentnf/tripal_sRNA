<?php

/**
 * @file
 * Contains all functions for creating the node for miRNA target prediction
 * the node should be mRNA miRNA sequence file
 */

/**
 * Implements hook_node_info().
 */
function mirtarget_node_info() {
  return array(
    'mipredseq' => array(
      'name' => t('miRNA/mRNA Sequence File'),
      'base' => 'mipredseq',
      'description' => t('Add a miRNA/mRNA (fasta) for miRNA target prediction.'),
    ),
  );
}

/**
 * Implements hook_node_access().
 */
function mirtarget_node_access($node, $op, $account) {
  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }

  if($node_type == 'mipredseq') {
    if ($op == 'create') {
      if (!user_access('create miRNA/mRNA Seq File', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit miRNA/mRNA Seq File', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete miRNA/mRNA Seq File', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access miRNA/mRNA Seq File', $account)) {
        return NODE_ACCESS_DENY;
      }
    }

    return NODE_ACCESS_IGNORE;
  }
}

/**
 * Form constructor for the mirtarget node
 *
 * @see mirtarget_insert()
 * @see mirtarget_update()
 * @see mirtarget_delete()
 * @see mirtarget_load()
 */
function mipredseq_form($node, &$form_state) {
  $form = array();

  $form['#validate'] = array('mipredseq_form_validate');
  
  $form['core'] = array(
    '#type' => 'fieldset',
    '#title' => 'General'
  );

  $form['core']['db_name']= array(
    '#type' => 'textfield',
    '#title' => t('Human-readable Name for miRNA/mRNA Seq File'),
    '#required' => TRUE,
    '#default_value' => isset($node->db_name) ? $node->db_name : '',
    '#description' => t('For example: watermelon mRNA (97103)'),
  );

  $form['core']['db_path']= array(
    '#type' => 'textfield',
    '#title' => t('File Prefix including Full Path'),
    '#description' => t('The full path to your seq file. For example, /home/web/datafile/sequence/wm_97103_mRNA.fasta'),
    '#required' => TRUE,
    '#default_value' => isset($node->db_path) ? $node->db_path : '',
  );

  $form['core']['db_type'] = array(
    '#type' => 'select',
    '#title' => t('Type of sequence'),
    '#description' => t('Please select the type of sequence from list'),
    '#required' => TRUE,
    '#options' => array(
      'mRNA' => 'mRNA',
      'miRNA' => 'microRNA',
    ),
    '#default_value' => isset($node->db_type) ? $node->db_type : '',
  );

  $form['core']['db_dblink'] = array(
    '#type' => 'textfield',
    '#title' => t('Basic link for link the IDs in seq file out to feature page'),
    '#description' => t('tripal.feilab.net/feature/mRNA/'),
    '#required' => TRUE,
    '#default_value' => isset($node->db_dblink) ? $node->db_dblink : '',
  );

  $form['core']['db_example_seq'] = array(
    '#type' => 'textarea',
    '#title' => t('Example of sequence for analysis'),
    '#required' => TRUE,
    '#default_value' => isset($node->db_example_seq) ? $node->db_example_seq : '',
    '#description' => t('The example sequence will be used for target prediction.'),
    '#cols' => 30,
    '#rows' => 10,
    '#maxlength' => 1000,
  );

  return $form;
}

function mipredseq_form_validate($form, $form_state) {

}

/**
 * Implements hook_insert().
 */
function mipredseq_insert($node) {
  
  // Actually insert the record.
  db_insert('mipredseq')->fields(array(
    'nid'     => $node->nid,
    'name'    => $node->db_name,
    'path'    => $node->db_path,
    'type'    => $node->db_type,
    'dblink'  => $node->db_dblink,
    'example' => $node->db_example_seq,
  ))->execute();
}

/**
 * Implements hook_node_insert().
 * This function acts on ALL NODES
 */
function mirtarget_node_insert($node) {
  if ($node->type == 'mipredseq') {
    $node->title = $node->db_name;
  }
}

/**
 * Implements hook_update().
 * modify pwy in database level
 */
function mipredseq_update($node) {
  // Update the record.
  db_update('mipredseq')->fields(array(
    'name'    => $node->db_name,
    'path'    => $node->db_path,
    'type'    => $node->db_type,
    'dblink'  => $node->db_dblink,
    'example' => $node->db_example_seq,
  ))->condition('nid', $node->nid)->execute();
}

/**
 * Implements hook_node_update().
 * This function acts on ALL NODES
 * for mirtarget_node
 */
function mirtarget_node_update($node) {
  if ($node->type == 'mipredseq')  {
    $node->title = $node->db_name;
  }
}

/**
 * Implements hook_delete(). 
 * modify mirtarget in database level 
 */
function mipredseq_delete($node) {
  db_delete('mipredseq')->condition('nid',$node->nid)->execute();
}

/**
 * Implements hook_load().
 * modify mirtarget in database level
 */
function mipredseq_load($nodes) {

  $sql = "
    SELECT nid, name, path, type, dblink, example
    FROM {mipredseq} 
    WHERE nid IN (:nids)";
  $result = db_query($sql, array(':nids' => array_keys($nodes)));

  foreach ($result as $record) {
    $nodes[$record->nid]->db_name = $record->name;
    $nodes[$record->nid]->db_path = $record->path;
    $nodes[$record->nid]->title   = $record->name;
    $nodes[$record->nid]->db_type = $record->type;
    $nodes[$record->nid]->db_dblink = $record->dblink;
    $nodes[$record->nid]->db_example_seq = $record->example;
  }
}

