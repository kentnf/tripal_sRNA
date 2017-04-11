<?php

/**
 * @file
 * Contains hooks to handle installation of this module.
 *
 */

/**
 * Implements hook_install().
 * create folder tripal_pathway under: default/files/tripal/
 */
function tripal_srna_install() {
   tripal_create_files_dir('tripal_mirtarget');
}

/**
 * Implements hook_schema().
 * Create the pwy table for storing pwy File related to pwy nodes.
 *
 */
function tripal_srna_schema() {

  // table for store pwy file name and path
  $schema['mipredseq'] = array(
    'description' => t('The table for pwy node'),
    'fields' => array(
      'nid' => array(
        'description' => t('The primary identifier for a node.'),
        'type' => 'serial', 'unsigned' => true, 'not null' => true,
      ),
      'name' => array(
        'description' => t('The human-readable name of miRNA/mRNA seq.'),
        'type' => 'varchar', 'length' => 255, 'not null' => true,
      ),
      'path' => array(
        'description' => t('The full path of the sequence file.'),
        'type' => 'varchar', 'length' => 1023, 'not null' => true,
      ),
      'type' => array(
        'description' => t('The type of sequence (miRNA or mRNA).'),
        'type' => 'varchar', 'length' => 10, 'not null' => true,
      ),
      'dblink' => array(
        'description' => t('base link of IDs in sequence file.'),
        'type' => 'varchar', 'length' => 1023, 'not null' => true,
      ),
      'example' => array(
        'description' => t('example sequences.'),
        'type' => 'text', 'not null' => true,
      ),
    ),
    'indexes' => array(
      'name' => array('name'),
    ),
    'primary key' => array('nid'),
    'unique keys' => array(
      'nid' => array('nid'),
    ),
  );

  $schema['srna_feature'] = array(
    'description' => t('sRNA sequence feature'),
    'fields' => array(
      'srna_id'     => array('type' => 'varchar', 'length' => 50, 'not null' => true),
      'sequence'    => array('type' => 'varchar', 'length' => 50, 'not null' => true),
      'length'      => array('type' => 'int', 'size' => 'small', 'not null' => true),
      'annotation'  => array('type' => 'varchar', 'length' => 255,
        'description' => t('The annotation of sRNA, such as miRNA, tRNA, siRNA ...'),
      ),
      'organism_id' => array('type' => 'int', 'size' => 'big', 'not null' => true),  
    ),

    'indexes' => array(
      'srna_id' => array('srna_id'),
    ),
    'primary key' => array('srna_id'),
    'unique keys' => array(
      'srna_id' => array('srna_id'),
    ),
  );

  $schema['srna_feature_expression'] = array(
    'description' => t('sRNA expression value'),
    'fields' => array(
      'express_id'     => array('type' => 'serial', 'unsigned' => true, 'not null' => true),
      'srna_id'        => array('type' => 'varchar', 'length' => 50, 'not null' => true),
      'experiment_id'  => array('type' => 'int', 'size' => 'big', 'not null' => true),
      'number_count'   => array('type' => 'int', 'size' => 'normal', 'not null' => true),
    ),
    'indexes' => array(
      'srna_id' => array('srna_id'),
      'experiment_id' => array('experiment_id'),
    ),
    'primary key' => array('express_id'),
    'unique keys' => array(
      'srna_sample_id' => array('srna_id', 'sample_id'),
    ),
    'foreign keys' => array(
      'srna_sample' => array(
        'table' => 'srna_sample',
        'columns' => array(
          'sample_id' => 'sample_id',
        ),
      ),
      'srna_feature' => array(
        'table' => 'srna_feature',
        'columns' => array(
          'srna_id' => 'srna_id',
        ),
      ),
    ),
  );

  // tables for miRNA
  //   -- including miRNA feature, miRNA conserved, miRNA_hairpin
  //      miRNA_star, miRNA_target
  $schema['mirna_feature'] = array(
    'description' => t('miRNA feature table'),
    'fields' => array(
      'mirna_id'    => array('type' => 'varchar', 'length' => 50, 'not null' => true),
      'srna_id'     => array('type' => 'varchar', 'length' => 50, 'not null' => true),
      'family'      => array('type' => 'varchar', 'length' => 50, 'description' => t('mirna family.')),
      'organism_id' => array('type' => 'int', 'size' => 'big', 'not null' => true),
    ),
    'indexes' => array(
      'mirna_id' => array('mirna_id'),
      'family' => array('family'),
    ),
    'primary key' => array('mirna_id'),
    'unique keys' => array(
      'mirna_id' => array('mirna_id'),
    ),
    'foreign keys' => array(
      'srna_feature' => array(
        'table' => 'srna_feature',
        'columns' => array(
          'srna_id' => 'srna_id',
        ),
      ),
    ),
  );

  // table for miRNA conserved
  $schema['mirna_conserved'] = array(
    'description' => t('miRNA and family alignment info'),
    'fields' => array(
      'cid'          => array('type' => 'serial', 'unsigned' => true, 'not null' => true),
      'mirna_id'     => array('type' => 'varchar', 'length' => 50, 'not null' => true),
      'hit_id'       => array('type' => 'varchar', 'length' => 50, 'not null' => true, 
                              'description' => t('mature miRNA id in miRBase.')),
      'align_query'  => array('type' => 'varchar', 'length' => 50, 'not null' => true),
      'align_string' => array('type' => 'varchar', 'length' => 50, 'not null' => true),
      'align_hit'    => array('type' => 'varchar', 'length' => 50, 'not null' => true),
    ),
    'indexes' => array(  
      'cid' => array('cid'),
    ),
    'primary key' => array('cid'),
    'unique keys' => array(
      'mirna_hit_id' => array('mirna_id', 'hit_id'),
    ),
    'foreign keys' => array( 
      'mirna_feature' => array(
        'table' => 'mirna_feature',
        'columns' => array(
          'mirna_id' => 'mirna_id',
        ),
      ),
    ),
  );

  // table for miRNA hairpin
  $schema['mirna_hairpin'] = array(
    'description' => t('pre-miRNA and hairpin structure'),
    'fields' => array(
      'hairpin_id'     => array('type' => 'varchar', 'length' => 50, 'not null' => true),
      'mirna_id'       => array('type' => 'varchar', 'length' => 50, 'not null' => true),
      'genome_id'      => array('type' => 'varchar', 'length' => 50, 'not null' => true),
      'hit_start'      => array('type' => 'int', 'size' => 'big', 'not null' => true),
      'hit_end'        => array('type' => 'int', 'size' => 'big', 'not null' => true),
      'mir_start'      => array('type' => 'int', 'not null' => true),
      'mir_end'        => array('type' => 'int', 'not null' => true),
      'strand'         => array('type' => 'varchar', 'length' => 2, 'not null' => true),
      'sequence'       => array('type' => 'varchar', 'length' => 255, 'not null' => true),
      'folding_energy' => array('type' => 'varchar', 'length' => 10, 'not null' => true),
    ),
    'indexes' => array(
      'hairpin_id' => array('hairpin_id'),
      'mirna_id' => array('mirna_id'),
    ),
    'primary key' => array('hairpin_id'),
    'unique keys' => array(
      'hairpin_id' => array('hairpin_id'),
    ),
    'foreign keys' => array(
      'mirna_feature' => array(
        'table' => 'mirna_feature',
        'columns' => array(
           'mirna_id' => 'mirna_id',
        ),
      ),
    ),
  );

  // table for miRNA star
  $schema['mirna_star'] = array(
    'fields' => array(
      'mir_star_id' => array('type' => 'serial', 'unsigned' => true, 'not null' => true),
      'hairpin_id'  => array('type' => 'varchar', 'length' => 50, 'not null' => true),
      'srna_id'     => array('type' => 'varchar', 'length' => 50, 'not null' => true),
      'srna_start'  => array('type' => 'int', 'not null' => true),
      'srna_end'    => array('type' => 'int', 'not null' => true),
    ),
    'primary key' => array('mir_star_id'),
    'unique keys' => array(
      'hairpin_srna_id' => array('hairpin_id', 'srna_id'),
    ),
    'foreign keys' => array(  
      'srna_feature' => array(
        'table' => 'srna_feature',
        'columns' => array(
          'srna_id' => 'srna_id',
        ),
      ),
      'mirna_hairpin' => array(
        'table' => 'mirna_hairpin',
        'columns' => array(
          'hairpin_id' => 'hairpin_id',
        ),
      ),
    ),
  );

  // table for miRNA target
  $schema['mirna_target'] = array(
    'fields' => array(
      'tid'          => array('type' => 'serial', 'unsigned' => true, 'not null' => true),
      'mirna_id'     => array('type' => 'varchar', 'length' => 50, 'not null' => true),
      'target_id'    => array('type' => 'varchar', 'length' => 255, 'not null' => true),
      'align_start'  => array('type' => 'int', 'not null' => true),
      'align_end'    => array('type' => 'int', 'not null' => true),
      'score'        => array('type' => 'varchar', 'length' => 10, 'not null' => true),
      'align_query'  => array('type' => 'varchar', 'length' => 255, 'not null' => true),
      'align_string' => array('type' => 'varchar', 'length' => 255, 'not null' => true),
      'align_hit'    => array('type' => 'varchar', 'length' => 255, 'not null' => true),
      'strand'       => array('type' => 'int', 'size' => 'small', 'not null' => true),
    ),
    'primary key' => array('tid'),
    'unique keys' => array(
      'mir_taget_id' => array('mirna_id', 'target_id'),
    ),
    'foreign keys' => array(
      'mirna_feature' => array(
        'table' => 'mirna_feature',
        'columns' => array(
          'mirna_id' => 'mirna_id',
        ),
      ),
    ),
  );

  return $schema;
}

