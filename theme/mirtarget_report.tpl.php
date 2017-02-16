<?php
/**
 * Display the miRNA target prediction result
 */
  if ($result_table) {

    $header_data = array( 'sRNA', 'mRNA', 'Alignment', 'Score', 'Mismatch', 'Wobble', 'Indel', 'mRNA Direction');

    $rows_data = array(); 
    foreach ($result_table as $line) 
    {
      $line_html = array();
      // sRNAID  mRNAID  start   end score   mRNA    sRNA    align   strand  mismatch    wobble  indel

      // check if the sRNA/mRNA has feature
      $sRNA_id = $line['sRNAID'];
      $mRNA_id = $line['mRNAID'];

      $sRNA_feature = chado_generate_var('feature', array('uniquename'=>$sRNA_id));
      $mRNA_feature = chado_generate_var('feature', array('uniquename'=>$mRNA_id));      
      if (!empty($sRNA_feature) && property_exists($sRNA_feature, 'nid')) {
        $sRNA_id = l($sRNA_id, "node/" . $sRNA_feature->nid, array('html' => TRUE));
      }
      if (!empty($mRNA_feature) && property_exists($mRNA_feature, 'nid')) {
        $mRNA_id = l($mRNA_id, "node/" . $mRNA_feature->nid, array('html' => TRUE));
      }

      // generate alignment
      $mRNA_seq = $line['mRNA'];
      $align    = $line['align'];
      $sRNA_seq = $line['sRNA'];

      $alignment = '<pre>';
      $alignment.= 'mRNA (' . $line['start'] . ' - ' . $line['end'] . ")\n";
      $alignment.= "mRNA 5' $mRNA_seq 3'\n";
      $alignment.= "        $align\n";
      $alignment.= "sRNA 3' $sRNA_seq 5'\n";
      $alignment.= '</pre>';

      $line_html[0] = $sRNA_id;
      $line_html[1] = $mRNA_id;
      $line_html[2] = $alignment;
      $line_html[3] = $line['score'];
      $line_html[4] = $line['mismatch'];
      $line_html[5] = $line['wobble'];
      $line_html[6] = $line['indel'];
      $line_html[7] = $line['strand'];
      $rows_data[] = $line_html;
    }
 
    $header = array(
      'data' => $header_data,
    );

    $rows = array(
      'data' => $rows_data,
    );

    $variables = array(
      'header' => $header_data,
      'rows' => $rows_data,
      'attributes' => array('class' => 'table vertical-align'),
    );

    print theme('table', $variables);
  }
