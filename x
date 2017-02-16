
Calling: run_mirtarget_tripal_job(miRNA_target_pred.pl, /tmp/2017Feb14_165807_miRPred.fasta, /home/web/sequence/watermelon_unigene_v2, sites/default/files/tripal/tripal_mirtarget/2017Feb14_165807.miRTargetPred.txt, a:7:{s:1:"q";s:5:"miRNA";s:1:"x";s:34:"tripal.feilab.net/feature/unigene/";s:1:"s";s:1:"3";s:1:"m";s:1:"3";s:1:"w";s:1:"2";s:1:"d";s:1:"0";s:1:"r";s:4:"both";}, 65)

Executing miRNA_target_pred.pl

Gene List: /tmp/2017Feb14_165807_miRPred.fasta
PWY File: /home/web/sequence/watermelon_unigene_v2
Results File: sites/default/files/tripal/tripal_mirtarget/2017Feb14_165807.miRTargetPred.txt
Invalid argument supplied for foreach() mirtarget.api.inc:103                                                               [warning]

Executing the following pwy enrichment command:
'miRNA_target_pred.pl' '/tmp/2017Feb14_165807_miRPred.fasta' '/home/web/sequence/watermelon_unigene_v2' 'sites/default/files/tripal/tripal_mirtarget/2017Feb14_165807.miRTargetPred.txt'
[blastall] FATAL ERROR: test1: Database /home/web/sequence/watermelon_unigene_v2 was not found or does not exist
[blastall] FATAL ERROR: test1: Database /home/web/sequence/watermelon_unigene_v2 was not found or does not exist
1-th input query: test1...Done
[blastall] FATAL ERROR: test2: Database /home/web/sequence/watermelon_unigene_v2 was not found or does not exist
[blastall] FATAL ERROR: test2: Database /home/web/sequence/watermelon_unigene_v2 was not found or does not exist
2-th input query: test2...Done
doneCalling: run_mirtarget_tripal_job(miRNA_target_pred.pl, /tmp/2017Feb16_153658_miRPred.fasta, /home/web/sequence/watermelon_unigene_v2, sites/default/files/tripal/tripal_mirtarget/2017Feb16_153658.miRTargetPred.txt, Array, 66)

Executing miRNA_target_pred.pl

Gene List: /tmp/2017Feb16_153658_miRPred.fasta
PWY File: /home/web/sequence/watermelon_unigene_v2
Results File: sites/default/files/tripal/tripal_mirtarget/2017Feb16_153658.miRTargetPred.txt
        q: miRNA
        x: tripal.feilab.net/feature/unigene/
        s: 3
        m: 3
        w: 2
        r: both

Executing the following pwy enrichment command:
'miRNA_target_pred.pl' -'q' 'miRNA' -'x' 'tripal.feilab.net/feature/unigene/' -'s' '3' -'m' '3' -'w' '2' -'r' 'both' '/tmp/2017Feb16_153658_miRPred.fasta' '/home/web/sequence/watermelon_unigene_v2' 'sites/default/files/tripal/tripal_mirtarget/2017Feb16_153658.miRTargetPred.txt'
[blastall] FATAL ERROR: test1: Database /home/web/sequence/watermelon_unigene_v2 was not found or does not exist
[blastall] FATAL ERROR: test1: Database /home/web/sequence/watermelon_unigene_v2 was not found or does not exist
1-th input query: test1...Done
[blastall] FATAL ERROR: test2: Database /home/web/sequence/watermelon_unigene_v2 was not found or does not exist
[blastall] FATAL ERROR: test2: Database /home/web/sequence/watermelon_unigene_v2 was not found or does not exist
2-th input query: test2...Done


perl miRNA_target_pred.pl miR.fa /home/web/datafile/blast_database/melon_unigene_v4 out

