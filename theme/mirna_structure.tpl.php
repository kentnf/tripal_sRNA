<?php

$a = 10;
dpm($a);

// <a href="$tmp_url/$imageFile" target=_blank><img class="img-responsive" src="$tmp_url/$imageFile"></a>

$head_data = array('Sequence', 'Length', 'Annotation');
$rows_data = array(array('x', 'y', 'z'));
$head = array('data' => $head_data);
$rows = array('data' => $rows_data);

$variables = array(
  'header' => $head_data,
  'rows' => $rows_data,
  'attributes' => array('class' => 'table vertical-align'),
);
print theme('table', $variables);

