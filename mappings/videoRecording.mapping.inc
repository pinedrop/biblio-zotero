<?php
function biblio_zotero_get_field_mappings__videoRecording() {
   return array(
      array(
         'source' => 'videoRecording:abstractNote',
         'target' => 'biblio_abst_e',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:accessDate',
         'target' => 'biblio_access_date',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:archive',
         'target' => 'biblio_publisher',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:archiveLocation',
         'target' => 'biblio_place_published',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:callNumber',
         'target' => 'biblio_call_number',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:date',
         'target' => 'biblio_year',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:extra',
         'target' => '',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:ISBN',
         'target' => 'biblio_isbn',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:language',
         'target' => 'biblio_lang',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:libraryCatalog',
         'target' => '',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:numberOfVolumes',
         'target' => '',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:place',
         'target' => 'biblio_place_published',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:rights',
         'target' => 'biblio_custom7',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:runningTime',
         'target' => '',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:seriesTitle',
         'target' => '',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:shortTitle',
         'target' => '',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:studio',
         'target' => '',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:title',
         'target' => 'title',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:url',
         'target' => 'biblio_url',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:videoRecordingFormat',
         'target' => '',
         'unique' => FALSE,
         ),
      array(
         'source' => 'videoRecording:volume',
         'target' => 'biblio_volume',
         'unique' => FALSE,
         ),
      );
}