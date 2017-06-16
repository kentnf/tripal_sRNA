<?php

?>
<div class="row"> <div class="col-md-8 col-md-offset-2">
<h3>Method Description</h3>
<p>
The small RNA datasets were combined by removing redundancies (same sequences,
same lengths, and same directions). Small RNAs derived from rRNA and tRNA were
discarded. sRNAs <b>with transcripts per million (TPM) >=50 in at least one 
sample and with length of 19-24 nt</b> were included in the miRNA identification.
These sRNAs were aligned to the correspinding genome sequence and the flanking
sequences (200bp to each side) of sRNAs with no more than 20 unique genome hits
were extracted and folded with the <a href="http://rna.tbi.univie.ac.at" target="_blank">RNAfold</a> program.
The structures were then checked with <a href="http://bartellab.wi.mit.edu/software.html" target="_blank">miRcheck</a>
to identify potential miRNA candidates. miRNA candidates were then compared to
<a href="http://www.mirbase.org" target="_blank">miRBase</a> to identify
conserved miRNA candidates (up to two mismatches). The miRNA targets were
identified using the <a href="/mirtarget" target="_blank">target prediction tool</a>
developed and implemented in the database.
</p>

<h3>small RNA Collection</h3>
<p>The small RNA database contains small RNAs collected from the following projects</p>

<?php

// get all organism
$organisms = tripal_srna_get_organism_options();

foreach ($organisms as $organism_id => $common_name) {

  // get project, sample, and experiment for this organism
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
  $sql = "SELECT E.experiment_id, E.name, T.project_id, T.name as project_name, S.biomaterial_id, 
     S.name as biomaterial_name 
    FROM chado.experiment E
    LEFT JOIN chado.biomaterial S ON E.biomaterial_id = S.biomaterial_id
    LEFT JOIN chado.project T ON E.project_id = T.project_id
    INNER JOIN chado.experimentprop P ON E.experiment_id = P.experiment_id
    WHERE P.type_id = :type_id AND S.taxon_id = :organism_id 
  ";
  $args = array(':type_id' => $type_id, ':organism_id' => $organism_id);
  $result = db_query($sql, $args)->fetchAll();

  // output result to table
  if (sizeof($result) > 0) {
    $header_data = array('Project', 'Sample', 'Description');
    $rows_data = array();
    foreach ($result as $exp) {
      $project = l($exp->project_name, 'bioproject/' . $exp->project_id);
      $sample = l($exp->biomaterial_name, 'biosample/' . $exp->biomaterial_id);
      $biomaterial = chado_generate_var('biomaterial', array('biomaterial_id' => $exp->biomaterial_id));
      $biomaterial = chado_expand_var($biomaterial, 'table', 'biomaterialprop', array('return_array' => 1));
      $desc = '';
      foreach ($biomaterial->biomaterialprop as $prop) {
        $desc.= $prop->type_id->name . " : " . $prop->value . "; ";
      }
      $rows_data[] = array($project, $sample, $desc);
    }
 
    $variables = array(
      'header' => $header_data,
      'rows' => $rows_data,
      'attributes' => array('class' => 'table vertical-align'),
    );
    print "<h4>$common_name</h4>";    
    print theme('table', $variables);
  }
}

?>


</div></div>
