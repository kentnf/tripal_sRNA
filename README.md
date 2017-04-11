


miRNA to feature table

orgid
miRNAid  -> name , uniquename          [P_M00018] -- 610248
miRNAseq -> residues, sequlen, md5     [UCCAAAGGGAUCGCAUUGAUC]
type_id  -> miRNA (name in cvterm, SO) [560] 

load pre_miRNA to feature table

pre_miRNA feature
pre_miRNAid  -> name, uniquename               [H000006] -- 610249
pre_miRNAseq -> residues, seqlen, md5          UCCAAAGGGAUCGCAUUGAUCCAAUGUUUGUCUUGUCUU
                                               AUCAAUGAUAUGUUGGAGUUGAUUCUCGAUCAAUUAUUGGAUCGUGCGAUCCCUUAGGA
type_id      -> pre_miRNA (name in cvterm, SO) [562] 

load miRNA and pre_miRNA to feature relationship table
 
subject_id : miRNA                             [610248] subject
object_id  : pre_miRNA                         [610249] object
type_id    : part_of (name in cvterm, SO)      [158] part of
                                               
   === I put the -42.60 of olding_energy to value of feature relationship table
 
   insert tye folding_energy to cvterm on install file

   load folding_energy to feature_replationshipprop table
     feature_relationship_id
     type_id : folding_energy (name in cvterm, tripal) 
     value   : -40.60
     rank    : 0  

load miRNA and pre_miRNA to feature location
feature_id   : miRNA                           [610248]
srcfeature_id: pre_miRNA                       [610249]
fmin: start                                    [1]
fmax: end                                      [21]
strand: -1, or 1,                              [1]

======== this contig is just a example ======== 
[610250] feature id 
[Scaffold000029] name and uniquename
[414] supercontig of cvterm

feature relationship
610249
610250
158

feature location
610249 
610250
1734514
1734611
1
=================================================

=================================================
Target 

feature relationship
   dbxref
     db_id [2] tripal
     accession [target_to]
     description [miRNA/siRNA target to other long RNA]
     get dbxref_id [95918]

   cvterm
     cv_id [5] tripal
     name [target_to] 
     definition miRNA/siRNA target to other long RNA
     dbxref_id [95918] [place dbxref_id to here] 
    
   the return cvterm id is : 48990 
 
++++ write the dbxref and cvterm into install file ++++

== feature relationship table 
[610248] P_M00018
[3]      orange1.1g015632m
[48990]  type
[2]      value for target score

== insert mirna target to other table
  structure
    mirna_id  [feature_id]
    target_id [feature_id]
    target_start
    target_end
    score
    align_query
    align_hit
    align_string
    strand

===============================================

== insert mirna conserved table
  structure
    mirna_id [feature_id]  unique
    hit_id   [just name]   unique
    align_query
    align_string
    align_hit

== insert mirna star table
  structure
    hairpin_id  unique
    srna_id     unique  index
    start
    end 


#Installation

bug1 why adjust p value is small than raw p value ? 


1. create soft link of miRNA_target_pred.pl to path. Example:
```
ln -s /var/www/html/sites/all/modules/tripal_miRNA/miRNA_target_pred.pl /usr/local/bin
```

2. install mirtarget module

```
drush pm-enable mirtarget
```

3. add node of miRNA or mRNA sequence

