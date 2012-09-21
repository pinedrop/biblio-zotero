<?php
function biblio_zotero_get_field_mappings__dictionaryEntry() {
   return array(
      array(
         'source' => 'dictionaryEntry:abstractNote',
         'target' => 'biblio_abst_e',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:accessDate',
         'target' => 'biblio_access_date',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:archive',
         'target' => 'biblio_publisher',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:archiveLocation',
         'target' => 'biblio_place_published',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:callNumber',
         'target' => 'biblio_call_number',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:date',
         'target' => 'biblio_year',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:dictionaryTitle',
         'target' => '',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:edition',
         'target' => 'biblio_edition',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:extra',
         'target' => '',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:ISBN',
         'target' => 'biblio_isbn',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:language',
         'target' => 'biblio_lang',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:libraryCatalog',
         'target' => '',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:numberOfVolumes',
         'target' => '',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:pages',
         'target' => 'biblio_pages',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:place',
         'target' => 'biblio_place_published',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:publisher',
         'target' => 'biblio_publisher',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:rights',
         'target' => 'biblio_custom7',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:series',
         'target' => '',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:seriesNumber',
         'target' => '',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:shortTitle',
         'target' => '',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:title',
         'target' => 'title',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:url',
         'target' => 'biblio_url',
         'unique' => FALSE,
         ),
      array(
         'source' => 'dictionaryEntry:volume',
         'target' => 'biblio_volume',
         'unique' => FALSE,
         ),
      );
}