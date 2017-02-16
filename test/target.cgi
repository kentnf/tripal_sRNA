#!/usr/bin/env perl
use lib '/var/www/cgi-bin/ICuGI/module/';
use CGI qw(:standard);
use CGI::Carp qw(fatalsToBrowser);
use ICuGI::sRNA;
use DBI;

$html = new ICuGI::sRNA();
$organism = param('organism');
$organism  =~ s![^A-Za-z0-9:_ @\.]+!!g; # 20160531, Added Sunhh.
print header();
print $html->header($organism);

$query = param("query_sequence");
$query  =~ s![^A-Za-z0-9:_ @\.]+!!g; # 20160531, Added Sunhh.
$key2 = param("score");
$key2  =~ s![^A-Za-z0-9:_ @\.]+!!g; # 20160531, Added Sunhh.
$key3 = param("mismatch");
$key3  =~ s![^A-Za-z0-9:_ @\.]+!!g; # 20160531, Added Sunhh.
$key4 = param("wobble");
$key4  =~ s![^A-Za-z0-9:_ @\.]+!!g; # 20160531, Added Sunhh.
$key5 = param("indel");
$key5  =~ s![^A-Za-z0-9:_ @\.]+!!g; # 20160531, Added Sunhh.
$choice1 = param("query");
$choice1  =~ s![^A-Za-z0-9:_ @\.]+!!g; # 20160531, Added Sunhh.
$choice2 = param("search_strand");
$choice2  =~ s![^A-Za-z0-9:_ @\.]+!!g; # 20160531, Added Sunhh.
$database = param("database");
$database  =~ s![^A-Za-z0-9:_ @\.]+!!g; # 20160531, Added Sunhh.

%database = ("pumpkin miRNA candidates" => "pumpkin_miRNA",
             "melon unigene" => "melon_unigene_v4",
             "cucumber unigene" => "cucumber_unigene_v3",
             "watermelon unigene" => "watermelon_unigene_v2",
             "cucurbita_pepo unigene" => "cucurbita_pepo_unigene_v1"
             );
@org = split("_", $database{$database});
if ($org[0] eq "cucurbita") { $org = "cucurbita_pepo"; }
else { $org = $org[0]; }

if ($query) {  # The path of BLAST tool
  $BLAST_PATH = "/usr/local/bin";

  # The path of database files
  #   - Save BLAST formatted database files of mRNAs and small RNAs
  #   - In case of small RNAs, please make format database files
  #     with their reverse complement

  $DATA_PATH = "/home/web/datafile/blast_database";

  # The path for temorary file
  $TEMP_PATH = "/var/www/html/ICuGI/tmp";

  # Default cutoff of target score ( score range: 0 < score <= 3 )
  $sc_cutoff = 4;
  if ($sc_cutoff > 4) { $sc_cutoff = 4; }

  # Cutoff value of alignment by dynamic programming
  $max_score_cutoff = 8;

  $blastdb_smRNA = $database{$database}."_rc";      #  smRNA 

  $blastdb_mRNA = $database{$database};        #  mRNA 

  # Source data files
  $mRNA_fas = $database{$database};
  $smRNA_fas = $database{$database};

  # Type of query sequence (smRNA: 0, mRNA: 1)
  $seq_type = 0;

  ###########################################################

  my @sel_list_disc = ();
  my @sel_list_pos_mRNA_st = ();
  my @sel_list_pos_sRNA_st = ();
  my @sel_list_pos_sRNA_end = ();
  my @sel_list_direction = ();
  my @description_list = ();

  # Temorary file for blast input
  $random = int(rand( )*1000000000000);
  $tempb = $TEMP_PATH.'/'."$random.tmp";

  sub insert_seq ($)

{    my $fn = $_[0];
    open(INFILE, $fn) or die("Fileed to read in $fn");

    my $seq_hash = {};
    my $seq = "";
    my $description;

    $count = 1;
    while($line = <INFILE>)

{      if ($line =~ m/^\>/)

{        if ($count > 1)

{          $seq_hash{$description} = $seq;
          $seq = "";
        }
        $description = split_discription($line);
        chomp $description;
      }
      else

{        chomp  $line;
        $seq .=  $line;
      }
      $count++;
    }
    close(INFILE);

    $seq_hash{$description} = $seq;
    return %seq_hash;
  }

  sub query_seq_from_file ($)

{    my $fn = $_[0];
    open(INFILE, $fn) or die("Fileed to read in $fn");

    my $seq = "";
    my $description;

    $count = 1;
    while($line = <INFILE>)

{      if ($line =~ m/^\>/)

{        if ($count > 2) { last; }
        $description = $line;
        chomp $description;
      }
      else

{        chomp  $line;
        $seq .=  $line;
      }
      $count++;
    }
    close(INFILE);

    @qseq = ($description, $seq);
    return @qseq;
  }

  sub get_query_seq ($)

{    my $text = $_[0];
    my @list = split(/\n/, $text);
    my $description;
    my $seq = "";
    my $qseq_hash = {};

    $count = 1;
    $count_desc = 1;
    foreach my $line(@list)
    {
      if ($line =~ m/^\>/)
      {
        if ($count > 1)
        {
          if (length($seq) < 10)
          {
            print "<br><h1 align=center>Please check the query sequence!</h1>";
            exit;
          }
          if ($count_desc >= 10)
          {
            print "<br><h1 align=center>Please check the number of query sequences (must be <= 10 )!</h1>";
            exit;
          }

          chomp($description);
          $seq =~ tr/atcg/ATCG/;
          $qseq_hash{$description} = $seq;
          push @description_list, $description;
          $seq = "";
          $count_desc++;
        }
        $description = split_discription($line);
        chomp $description;
      }
      else
      {
        if ($count eq 1)
        {
          $description = '>sample';
          chomp  $line;
          $line =~ s/^\s+//;
          $line =~ s/\s+$//;
          $seq .=  $line;
        }

        $line =~ s/^\s+//;
        $line =~ s/\s+$//;
        chomp  $line;
        $seq .=  $line;
      }
      $count++;
    }
    chomp($description);
    $seq =~ tr/atcg/ATCG/;
    $qseq_hash{$description} = $seq;
    push @description_list, $description;
    return (%qseq_hash);
  }

  sub reverse_complement
  {
    my $seq = shift;
    $seq =~ s/U/T/g;
    $seq =~ s/u/T/g;
    $seq = reverse($seq);
    $seq =~ tr/atcgATCG/tagcTAGC/;
    return $seq;
  }

  # Local sequence alignment
  sub lsa
  {
    my ($adisc, $bdisc, $mRseq, $smRseq, $pos, $direction) = @_;
    my $match = 1;
    my $wobble = 0.5;
    my $mismatch = -1;
    my $indel = -2;
    my $first_i = 0;
    my $first_j = 0;
    my $last_i = 0;
    my $last_j = 0;

    my $offset = 3;
    my $mRNA_st = $sel_list_pos_mRNA_st[$pos]+0;
    my $sRNA_st = $sel_list_pos_sRNA_st[$pos]+0;
    my $sRNA_end = $sel_list_pos_sRNA_end[$pos]+0;
    my $start = $mRNA_st - ($sRNA_st + $offset);
    if ($direction eq '-')
    {
      $start = $mRNA_st - ((length($smRseq)-$sRNA_end) + $offset);
    }
    if ($start < 0) { $start = 0; }
    my $seqlen = length($smRseq)+$offset;
    my $end = $start+$seqlen;
    if ($end >= length($mRseq)) { $seqlen = length($mRseq)-$start+1; }
    my $a = substr($mRseq, $start, $seqlen);
    my $b = $smRseq;
    my $S = [];   # An array of scores
    my $R = [];   # An array of backtracking arrows
    my $n = length($a);
    my $m = length($b);

    # We need to work in letters, not in strings.  This is a simple way
    # to turn a string of letters into an array of letters.
    my @a = split // => $a;
    my @b = split // => $b;

    # These are "constants" which indicate a direction in the backtracking array.
    my $UP_AND_LEFT = "\\";
    my $UP          = "|";
    my $LEFT        = "-";
    my $NEITHER     = "o";
    my $max_score = 0;
    my $sc = 0.0;
    my $mc = 0;
    my $wc = 0;
    my $ic = 0;
    my $answer = 0;
    my $bind = '';

    # Initialization
    my $score = 0;
    for(my $j = 0; $j <= $m; $j++)
    {
      $S->[0][$j] = 0;
      $R->[0][$j] = $NEITHER;
    }

    # This is the main dynamic programming loop that computes the score.
    for(my $i = 1; $i <= $n; $i++)
    {
      $S->[$i][0] = 0;
      $R->[$i][0] = $NEITHER;
      for(my $j = 1; $j <= $m; $j++)
      {
        $S->[$i][$j] = 0;
        $R->[$i][$j] = $NEITHER;
        if ( $a[$i-1] eq $b[$j-1] ) { $score = $match; }
        elsif ( is_wobble($a[$i-1], $b[$j-1], $direction) eq 1 ) { $score = $wobble; }
        else { $score = $mismatch; }

        if (($S->[$i-1][$j-1]+$score) > $S->[$i][$j])
        {
          $S->[$i][$j] = $S->[$i-1][$j-1]+$score;
          $R->[$i][$j] =$UP_AND_LEFT;
        }
        if (($S->[$i][$j-1]+$indel) > $S->[$i][$j])
        {
          $S->[$i][$j] = $S->[$i][$j-1]+$indel;
          $R->[$i][$j] =$LEFT;
        }
        if (($S->[$i-1][$j]+$indel) > $S->[$i][$j])
        {
          $S->[$i][$j] = $S->[$i-1][$j]+$indel;
          $R->[$i][$j] =$UP;
        }
        if ($S->[$i][$j] > $max_score)
        {
          $max_score = $S->[$i][$j];
          $last_i = $i;
          $last_j = $j;
        }
      }
    }
    my $seqa = '';
    my $seqb = '';

    if ( $max_score > $max_score_cutoff )
    {
      $i = $last_i;
      $j = $last_j;

      # Trace the backtracking matrix.
      while( $i > 0 and $j > 0 )
      {
        if( ($R->[$i][$j] eq $UP_AND_LEFT) and (($a[$i-1] eq $b[$j-1]) or (is_wobble($a[$i-1], $b[$j-1], $direction ) eq 1 )) )
        {
          $seqa = $a[$i-1].$seqa;
          $seqb = $b[$j-1].$seqb;
          $i--; $j--;
        }
        elsif( $R->[$i][$j] eq $LEFT )
        {
          $seqa = '-'.$seqa;
          $seqb = $b[$j-1].$seqb;
          $j--;
        }
        elsif( $R->[$i][$j] eq $UP )
        {
          $seqa = $a[$i-1].$seqa;
          $seqb = '-'.$seqb;
          $i--;
        }
        else
        {
          $seqa = $a[$i-1].$seqa;
          $seqb = $b[$j-1].$seqb;
          $i--; $j--;
        }
      }
      $first_i = $i;
      $first_j = $j;

      # Insert postfixes of seqb
      for(my $j = 1; $j <= length($smRseq)-$last_j; $j++)
      {
        $seqb = $seqb.$b[$last_j+$j-1];
      }
      my $lp = get_last_pos($first_i, $seqa);

      # Insert prefixes
      my $k = 1;
      my $ind = 0;
      for(my $j = $first_j-1; $j >= 0; $j--)
      {
        if (($first_i-$k) >= 0)
        {
          $seqa = $a[$first_i-$k-1].$seqa;
        }
        else
        {
          $seqa = '-'.$seqa;
          $ind = 1;
        }
        $seqb = $b[$j].$seqb;
        $k++;
      }
      if ($ind eq 1) { $first_i = 0; }
      else { $first_i = $first_i-$k+1; }
      $first_j = $first_j-$k-1;
      # Delete postfix indels
      for(my $i = length($seqa)-1; $i >= 0; $i--)
      {
        if ( substr($seqa,$i,1) eq '-' )
        {
          substr($seqa,$i,1) = '';
         }
        else { last; }
      }
      chomp($seqa);

      # Insert postfixes
      $k = 1;
      for(my $i = length($seqa); $i < length($seqb); $i++)
      {
        if (($lp+$k) < length($a) )
        {
          $seqa = $seqa.$a[$lp+$k];
        }
        else
        {
          $seqa = $seqa.'-';
        }
        $k++;
      }

      # Scoring of binding pair
      my $ca = '';
      my $cb = '';
      for(my $i = 0; $i < length($seqb); $i++)
      {
        $ca = substr($seqa,$i,1);
        $cb = substr($seqb,$i,1);
        if ( is_match($ca, $cb) eq 1 ) { $bind = $bind.'|'; }
        elsif ( is_wobble($ca, $cb, $direction) eq 1 ) { $wc++; $sc=$sc+0.5; $bind = $bind.'o'; }
        elsif ( is_indel($ca, $cb) eq 1 ) { $ic++; $sc=$sc+2.0; $bind = $bind.'b'}
        else  { $sc=$sc+1.0;  $mc++; $bind = $bind.'b'; }
      }
      if ( $sc <= $sc_cutoff ) { $answer = 1; }
    }
    @seqinfo = ($answer, $sc, $mc, $start+$first_i+1, $first_j+1, $seqa, $seqb, $bind, $adisc, $bdisc, $direction, $wc, $ic);
    return @seqinfo;
  }

  sub get_last_pos (@)
  {
    my ($start, $seq) = @_;
    my $barc = 0;
    for(my $i = 0; $i < length($seq); $i++)
    {
      if (substr($seq,$i,1) eq "-") { $barc++; }
    }
    my $last_pos = $start+length($seq)-$barc-1;
    return $last_pos;
  }

  sub is_match (@)
  {
    my ($ca, $cb) = @_;
    my $answer = 0;
    if ( $ca eq $cb ) { $answer = 1; }
    return $answer;
  }

  sub is_wobble (@)
  {
    my ($ca, $cb, $strand) = @_;
    my $answer = 0;
    if ( $strand eq $pos_direction )
    {
      if ( (($ca eq 'T') and ($cb eq 'C')) or (($ca eq 'G') and ($cb eq 'A' )) )
      {
        $answer = 1;
      }
    }
    else
    {
      if ( (($ca eq 'C') and ($cb eq 'T' )) or (($ca eq 'A') and ($cb eq 'G')) )
      {
        $answer = 1;
      }
    }
    return $answer;
  }

  sub is_indel (@)
  {
    my ($ca, $cb) = @_;
    my $answer = 0;
    if ( (($ca eq '-') and ($cb ne '-')) or (($ca ne '-') and ($cb eq '-' )) )
    {
      $answer = 1;
    }
    return $answer;
  }

  sub is_mismatch (@)
  {
    my ($ca, $cb) = @_;
    my $answer = 0;
    if ( $ca ne $cb ) { $answer = 1; }
    return $answer;
  }

  sub change_sequences (@)
  {
    my ($sad, $sbd, $sa, $sb, $is_mRNA) = @_;
    return   ($sad, $sbd, $sa, $sb);
  }

  sub split_discription ($)
  {
    my $sd = $_[0];
    my @list = split(' ', $sd);
    my $title = $list[0];
    return $title;
  }

  sub blast_input_save (@)
  {
    my ($disc, $qseq, $is_mRNA) = @_;
    my $answer = 1;
    if ( $is_mRNA eq 0 )
    {
      $qseq = reverse_complement($qseq);
    }
    chomp($disc);
    chomp($qseq);
    my $result_save = join "\n", $disc, $qseq;
    open OUTFILE, ">$tempb";
    print OUTFILE "$result_save";
    close OUTFILE;
    return $answer;
  }

  sub blast_run (@)
  {
    my ($blastdb, $queryfile, $direction) = @_;
    my $blastdbname = $DATA_PATH.'/'.$blastdb;
    my $blast_out = `$BLAST_PATH/blastall -p blastn -d $blastdbname -i $queryfile -W 7 -q -1 -S $direction -e 100 -m 8`;
    return $blast_out;
  }

  sub blast (@)
  {
    my ($query_disc, $query_seq, $queryf, $is_mRNA, $search_direction) = @_;
    my $i = 0;
    my $db = '';
    my $forward = 1;
    my $reverse = 2;
    $pos_direction = "+";
    $neg_direction = "-";
    if ( $is_mRNA eq 1 )
    {
      $db = $blastdb_smRNA;
      $rv = blast_input_save($query_disc, $query_seq, $is_mRNA);
      if ( ($search_direction eq 'forward') or ($search_direction eq 'both') )
      {
        $result = blast_run($db, $queryf, $forward);
        my @list = split(/\n/, $result);
        foreach my $line (@list)
        {
          @entry = split(/\t/,$line);
          $name = '>'.$entry[1];
          push @sel_list_disc, $name;
          push @sel_list_pos_mRNA_st, $entry[6];
          push @sel_list_pos_sRNA_st, $entry[8];
          push @sel_list_pos_sRNA_end, $entry[9];
          push @sel_list_direction, $pos_direction;
          $i++;
        }
      }
      if ( ($search_direction eq 'reverse') or ($search_direction eq 'both') )
      {
        $result = blast_run($db, $queryf, $reverse);
        @list = split(/\n/, $result);
        foreach my $line (@list)
        {
          @entry = split(/\t/,$line);
          $name = '>'.$entry[1];
          push @sel_list_disc, $name;
          push @sel_list_pos_mRNA_st, $entry[6];
          push @sel_list_pos_sRNA_st, $entry[9];
          push @sel_list_pos_sRNA_end, $entry[8];
          push @sel_list_direction, $neg_direction;
          $i++;
        }
      }
    }
    else
    {
      $db = $blastdb_mRNA;
      $rv = blast_input_save($query_disc, $query_seq, $is_mRNA);
      if ( ($search_direction eq 'forward') or ($search_direction eq 'both') )
      {
        $result = blast_run($db, $queryf, $forward);
        my @list = split(/\n/, $result);
        foreach my $line (@list)
        {
          @entry = split(/\t/,$line);
          $name = '>'.$entry[1];
          push @sel_list_disc, $name;
          push @sel_list_pos_mRNA_st, $entry[8];
          push @sel_list_pos_sRNA_st, $entry[6];
          push @sel_list_pos_sRNA_end, $entry[7];
          push @sel_list_direction, $pos_direction;
          $i++;
        }
      }

      if ( ($search_direction eq 'reverse') or ($search_direction eq 'both') )
      {
        $result = blast_run($db, $queryf, $reverse);
        @list = split(/\n/, $result);
        foreach my $line (@list)
        {
          @entry = split(/\t/,$line);
          $name = '>'.$entry[1];
          push @sel_list_disc, $name;
          push @sel_list_pos_mRNA_st, $entry[9];
          push @sel_list_pos_sRNA_st, $entry[6];
          push @sel_list_pos_sRNA_end, $entry[7];
          push @sel_list_direction, $neg_direction;
          $i++;
        }
      }
    }
    return $i;
  }

  sub get_lsa_result (@)
  {
    my  ($istarget, $sc, $mc, $sa, $sb, $seqa, $seqb, $bind, $adisc, $bdisc, $direction, $wc, $ic) = @_;
    $seqa =~ tr/T/U/;
    my $la = get_last_pos($sa, $seqa);
    my $summary = "";
    $summary .= "\n$adisc\n";
    $summary .= "$bdisc\n";
    $summary .= "\nScore: $sc\n";
    $summary .= "Mismatch: $mc\n";
    $summary .= "Wobble: $wc\n";
    $summary .= "Indel: $ic\n";
    $summary .= "Direction: $direction\n";
    if ( $direction eq '-')
    {
      $seqa = reverse_complement($seqa);
      $seqb = reverse($seqb);
      $seqa =~ tr/T/U/;
      $seqb =~ tr/T/U/;
      $bind = reverse($bind);
      $summary .= "\n$seqa (site: $la \- $sa)\n";
      $summary .= "$bind\n";
      $summary .= "$seqb\n";
    }
    else
    {
      $seqb =~ tr/atcgATCG/uagcUAGC/;
      $summary .= "\n$seqa (site: $sa \- $la)\n";
      $summary .= "$bind\n";
      $summary .= "$seqb\n";
    }
    return $summary;
  }

  sub file_out (@)
  {
    my ($ofn, $result, $result_tab) = @_;
    open OUTFILE, ">$ofn";
    print OUTFILE "$result";
    close OUTFILE;
    my $tabfn = $ofn."_tab";
    open OUTFILE, ">$tabfn";
    print OUTFILE "$result_tab";
    close OUTFILE;
  }

  sub html_out (@)
  {
    my ($description, $schash, $resulthash) =  @_;
    my $i = 0;
    my $summary = "";
    my $RNA = "mRNA";
    my $QUERY = "sRNA";
    if ($seq_type eq 1) { $RNA = "sRNA"; $QUERY = "mRNA";}
    $th_sc = $key2+0.0;
    $th_mismatch = $key3+0;
    $th_wobble = $key4+0;
    $th_indel = $key5+0;
    foreach $value (sort {$schash{$a} <=> $schash{$b}} keys %schash)
    {
      my  ($istarget, $sc, $mc, $sa, $sb, $seqa, $seqb, $bind, $adisc, $bdisc, $direction, $wc, $ic) = @listlsa = @{$resulthash{$value}};
      if ( (($th_sc >= $sc) and ($sc <= 6.0) and $sc > 0) and (($th_mismatch >= $mc) and ($mc <= 8)) and (($th_wobble >= $wc) and ($wc <= 5)) and (($th_indel >= $ic) and ($ic <= 2)) )
      {
        if ( $i eq 0 )
        {
          print qq'
          <table border=0 align=center width=650 cellpadding=3 cellspacing=0 style=border-collapse:collapse bordercolor=#000000>
          <tr bgcolor=#FFFFFF><td><B><font color=brown>$QUERY: $description</font></B></td></tr></table>
          <table border=1 align=center width=650 cellpadding=3 cellspacing=0 style=border-collapse:collapse bordercolor=#000000>
          <tr bgcolor=#E2E8EC><th>$RNA</th><th width=300>Alignment</th><th>Score</th><th>Mismatch</th><th>Wobble</th><th>Indel</th><th>mRNA<br>Direction</th></tr>';
        }
        $seqa =~ tr/T/U/;
        my $la = get_last_pos($sa, $seqa);
        print "<tr>";
        if ($seq_type eq 1) 
        { 
          $bdisc =~ s/>//;
          print "<td><a href=miRNA.cgi?ID=$bdisc&organism=$org>$bdisc</a></td>"; 
        }
        else 
        { 
          $adisc =~ s/>//; 
          print "<td><a href=/cgi-bin/ICuGI/EST/search.cgi?organism=$org&searchtype=unigene&unigene=$adisc>$adisc</a></td>";
        }
        if ( $direction eq '-')
        {
          $seqa = reverse_complement($seqa);
          $seqb = reverse($seqb);
          $seqa =~ tr/T/U/;
          $seqb =~ tr/T/U/;
          $bind = reverse($bind);
          $bind =~ s/b/x/g;
          print qq'<td><font face="Courier"><pre>mRNA ($la \- $sa)
mRNA 5\'$seqa 3\'
       $bind
sRNA 3\'$seqb 5\'</pre></font></td>
';
        }
        else
        {
          $seqb =~ tr/atcgATCG/uagcUAGC/;
          $bind =~ s/b/x/g;
          print qq'<td><font face="Courier"><pre>mRNA ($sa \- $la)
mRNA 5\'$seqa 3\'
       $bind
sRNA 3\'$seqb 5\'</pre></font></td>
';
        }
        print "<td align=center>$sc</td>";
        print "<td align=center>$mc</td>";
        print "<td align=center>$wc</td>";
        print "<td align=center>$ic</td>";
        print "<td align=center>$direction</td>";
        print "</tr>";
        $i++;
      }
    }
    if ( $i > 0 ) { print qq'</table><br>'; }
    else
    {
      print qq'<table border=0 align=center width=650 cellpadding=3 cellspacing=0 style=border-collapse:collapse bordercolor=#000000>
              <tr bgcolor=#FFFFFF><td><B><font color=brown>$QUERY: $description</font> - No interaction was identified.</B></td></tr></table><br>';
    }
  }

  sub sorting_sc
  {
    my ($schash, $resulthash) =  @_;
    my $summary = "";
    my $i = 1;
    foreach $value (sort {$schash{$a} <=> $schash{$b}} keys %schash)
    {
      my  @listlsa = @{$resulthash{$value}};
      $summary .= "\n=================<  $i  >=======================\n";
      $summary .= get_lsa_result(@listlsa);
      $i++;
    }
    return $summary;
  }

  sub print_tab
  {
    my $resulthash =  @_;
    my $summary = "";
    my $i = 1;
    foreach $value (sort {$schash{$a} <=> $schash{$b}} keys %schash)
    {
      my  @listlsa = @{$resulthash{$value}};
      my  $lp = get_last_pos($listlsa[3], $listlsa[5]);
      $summary .= "$i\t$listlsa[8]\t$listlsa[9]\t$listlsa[1]\t$listlsa[2]\t$listlsa[11]\t$listlsa[12]\t$listlsa[5]\t$listlsa[6]\t$listlsa[7]\t$listlsa[3]\t$lp\t$listlsa[10]\n";
      $i++;
    }
    return $summary;
  }

  sub run_target_score
  {
    my $source_file = "";
    if ($choice1 eq 'mRNA') { $seq_type = 1; }
    else { $seq_type = 0; }
    if ($seq_type eq 1) { $source_file = $DATA_PATH.'/'.$smRNA_fas; }
    else { $source_file = $DATA_PATH.'/'.$mRNA_fas; }
    my %seqsa = insert_seq($source_file);
    my %seq_querys = get_query_seq($query);
    my $direction = "+";
    my $search_direction = "both";
    if ($choice2 eq 'forward') { $search_direction = "forward"; }
    elsif ($choice2 eq 'reverse') { $search_direction = "reverse"; }
    $count_desc = 1;
    my $resulthash = {};
    my $schash = {};
    my @index_list = ();
    foreach my $description (@description_list)
    {
      my @seqb = ($description, $seq_querys{$description});
      $seq_name = $description;
      $seq_name =~ s/>//;
      if ( (length($seqb[1]) <= 30) and ($seq_type eq 1) ) 
      { 
        print "<br><table align=center width=650><tr><td><font color=red><b>Sequence $seq_name has worng type or format!</b></font></td></tr></table>"; 
        print $html->footer();
        exit;
      }
      if ( (length($seqb[1]) > 30) and ($seq_type eq 0) ) 
      {
        print "<br><table align=center width=650><tr><td><font color=red><b>Sequence $seq_name has wrong type or format!</b></font></td></tr></table>";
        print $html->footer();
        exit;
      }

      my $smRNA = "";
      my $smRNArc = "";
      my $mRNA = "";
      my $mRNAdisc = "";
      my $smRNAdisc = "";
      @sel_list_disc = ();
      @sel_list_pos_mRNA_st = ();
      @sel_list_pos_sRNA_st = ();
      @sel_list_pos_sRNA_end = ();
      @sel_list_direction = ();
      my $count = blast($seqb[0], $seqb[1], $tempb, $seq_type, $search_direction);
      my $i = 0;
      foreach my $disc (@sel_list_disc)
      {
        $direction = $sel_list_direction[$i];
        $mRNAdisc = split_discription($mRNAdisc);
        if ( $seq_type eq 1 )
        {
          ($mRNAdisc, $smRNAdisc, $mRNA, $smRNA) = change_sequences($seqb[0], $disc, $seqb[1], $seqsa{$disc}, $seq_type);
        }
        else
        {
          ($mRNAdisc, $smRNAdisc, $mRNA, $smRNA) = change_sequences($disc, $seqb[0], $seqsa{$disc}, $seqb[1], $seq_type);
        }
        if ( $direction eq "+" ) { $smRNArc = reverse_complement($smRNA); }
        else { $smRNArc = $smRNA; }
        chomp $smRNArc;
        chomp $mRNA;
        my @sinfo = lsa($mRNAdisc, $smRNAdisc, $mRNA, $smRNArc, $i, $direction);
        if ( ($sinfo[0] eq 1) )
        {
          my $start_pos=sprintf("%d", $sinfo[3]);
          my $index = join "_", $mRNAdisc, $smRNAdisc, $start_pos;
          $resulthash{$index} = \@sinfo;
          $schash{$index} = $sinfo[1];
          push @index_list, $index;
        }
        $i++;
      }
      $count_desc++;
      my $query_n = $description;
      $query_n =~ s/^\>//;
      html_out($query_n, %schash, %resulthash);
      foreach my $index (@index_list)
      {
        delete $resulthash{$index};
        delete $schash{$index};
      }
      @index_list = ();
    }
  }
        print qq'<h3 align=center>miRNA target prediction</h3>';
  run_target_score;
  unlink($tempb);
}
else
{  
  print qq'
  <script language="JavaScript">
  function verify(param1)
  {
    var param1valid = (param1.value != "");
    var isvalid = (param1valid);
    if (!isvalid)
    {
      alert("Please input a sequence!");
      param1.focus();
      return false
    }
    else
      return true
  }
  function my_win()
  {
    var url = "/ICuGI/include/target_help.html";
    var window_name = "mywindow";
    window.open(url,window_name,"width=550,height=480");
  }
  function clearText(thefield)
  {
    if (thefield.defaultValue==thefield.value)
    {
      thefield.value = "";
    }
  }
  var selection = new DynamicOptionList();
  selection.addDependentFields("query","database");
  selection.forValue("sRNA").addOptions("melon unigene", "cucumber unigene", "watermelon unigene", "cucurbita_pepo unigene");
  selection.forValue("mRNA").addOptions("pumpkin miRNA candidates");

  </script>
  <br><table align=center width=400 border=0 cellspacing=0 cellpadding=0>
  <tr><td width=10%>&nbsp;</td><td><p align=center class=head><b>$organism miRNA target prediction</b></p></td>
  <td width=10% align=right><a href=javascript:my_win()><b>Help</b></a></td><tr></table><br>

  <form name=keyword method=POST onSubmit="return verify(this.key1)">
  <input type=hidden name=organism value=$organism>
  <table align=center width=400 border=1 cellspacing=0 cellpadding=10 style=border-collapse:collapse bordercolor=#999999><tr><td>
  <table border=0 cellspacing=4 align=center>
  <tr><td><b>Input query type:</b> </td><td><select name="query">
  <option value="sRNA" selected>microRNA
  <option value="mRNA">mRNA
  </select></td></tr>
  <tr><td><b>Choose the database:</b></td><td><select name=database>
  <SCRIPT>selection.printOptions("database")</SCRIPT></select></td></tr>
  <tr><td colspan=2><b>Input sequence in fasta format (<font color=red>10 sequences maximum</font>):</b></td></tr>
  <tr><td colspan=2><textarea rows="5" cols="40" name="query_sequence" onfocus="clearText(this);">>test1
AGAAUCUUGAUGAUGCUGCAU
>test2
UCGGACCAGGCUUCAUUCCUC</textarea></td></tr>
  <tr><td><b>Score:</b></td><td><input type="text" size="10" name="score" value="3"> (0 ~ 4)</td></tr>
  <tr><td><b>Mismatch: </b></td><td><input type="text" size="10" name="mismatch" value="3"> (0 ~ 4)</td></tr>
  <tr><td><b>G:U Wobble:</b></td><td><input type="text" size="10" name="wobble" value="2"> (0 ~ 5)</td></tr>
  <tr><td><b>Indel:</b></td><td><input type="text" size="10" name="indel" value="0"> (0 ~ 1)</td></tr>
  <tr><td><b>Strand:</b></td><td><select name="search_strand">
  <option value="both" selected>Both
  <option value="forward">Forward
  <option value="reverse">Reverse
  </select></td></tr>
  <tr><td colspan=2 align="center"><br>
  <input type="submit" name="Submit" value="Submit" style="background-color:lightgreen">&nbsp;&nbsp;&nbsp;
  <input type="reset" name="Reset" value="Reset" style="background-color:lightgreen"></td></tr>
  </table></td></tr></table></form>
  ';
}
print $html->footer();
