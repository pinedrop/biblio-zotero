<?php
function writeMappingFiles() {
   include_once "ZoteroBiblioMap.inc";
   $map = ZoteroBiblioMap::getZoteroToBiblioFields();
   $lastItemType = 'artwork';
   foreach( self::getZoteroItemFields() as $field ) {
      $row = explode(":", $field);
      $itemType = $row[0];
      
      $fieldName = $row[1];
      $fn = "mappings/" . $itemType . ".mapping.inc";
      
      if ( ! file_exists($fn) ) {
         $header = "<?php
         function biblio_zotero_get_field_mappings__$itemType() {
         return array(";
         file_put_contents( $fn, $header );
      }       
      $source = $field;
      $target = isset($map[$fieldName]) ? $map[$fieldName] : null;
      $mapping = PHP_EOL . "array(
      'source' => '$field',
      'target' => '$target',
      'unique' => FALSE,
      ),";
      file_put_contents( $fn, $mapping, FILE_APPEND);
      
      echo $itemType, $lastItemType, PHP_EOL;
      if ( $itemType != $lastItemType ) {
         $lastFileName = "mappings/" . $lastItemType . ".mapping.inc";
         echo "writing footer", PHP_EOL;
         $footer ="\n);\n}";
         file_put_contents( $lastFileName, $footer, FILE_APPEND);
      }
      $lastItemType = $itemType;
   }
   echo "writing very last footer for last item type", PHP_EOL;
   $footer ="\n);\n}";
   file_put_contents( $lastFileName, $footer, FILE_APPEND);
   
}

