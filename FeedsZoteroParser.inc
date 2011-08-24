<?php

/**
 * Class definition for Zotero Parser.
 *
 * Parses RSS and Atom feeds.
 */
class FeedsZoteroParser extends FeedsSyndicationParser {

  /**
   * Implementation of FeedsParser::parse().
   */
  public function parse(FeedsImportBatch $batch, FeedsSource $source) {
    feeds_include_library('common_syndication_parser.inc', 'common_syndication_parser');
    $result = common_syndication_parser_parse($batch->getRaw());
    dd($result, "result");
    
    $batch->title = $result['title'];
    $batch->description = $result['description'];
    $batch->link = $result['link'];
    if (is_array($result['items'])) {
      foreach ($result['items'] as $item) {
         $json = $item['description'];
         $data = json_decode($json);
         $result = array_merge($item, (array)$data);
         $result['description'] = ''; //description gets mapped to Full Text of the biblio node so leave it blank
         $batch->items[] = $result;
      }
    }
  }

  /**
   * Return mapping sources.
   *
   * At a future point, we could expose data type information here,
   * storage systems like Data module could use this information to store
   * parsed data automatically in fields with a correct field type.
   */
  public function getMappingSources() {

     
     $sources = array(
     ) + parent::getMappingSources();
     
     return $sources;
  }
}
