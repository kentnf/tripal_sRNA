<?php

drupal_set_title('miRNA* of ' . $hairpin->hairpin_id . ' (' . $hairpin->mirna_id . ')');

$seq_length = 60;

foreach ($stars as $star) {

  $head_html = '<b>star sRNA: ';
  $head_html.= l($star->srna_id, 'srna/display/' . $star->srna_id);
  $head_html.= " (<font color=#0000FF>AUCG</font>: miRNA* | <font color=#FF0000>AUCG</font>: miRNA)</b>"; 

  print $head_html;

  $s = str_split($hairpin->sequence, 1);
  $n = 0;
  $seq_html = '<pre>';
  foreach ($s as $str) {
    ++$n;
    if ($n == $hairpin->mir_start) {
      $seq_html.= '<font color=#FF0000>';
    }
    if ($n == $star->srna_start) {
      $seq_html.= '<font color=#0000FF>';
    }
    $seq_html.= $str;
    if ($n == $hairpin->mir_end) {
      $seq_html.= '</font>';
    }
    if ($n == $star->srna_end) {
      $seq_html.= '</font>';
    }

    if ($n % $seq_length == 0) {
      $seq_html.= "\n";
    }
  }
  $seq_html.= '</pre>';  
  print $seq_html;
}

