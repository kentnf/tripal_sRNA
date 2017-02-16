
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

