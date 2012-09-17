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
   public function parse(FeedsSource $source, FeedsFetcherResult $fetcher_result) {
      feeds_include_library('common_syndication_parser.inc', 'common_syndication_parser');
      $feed_xml = simplexml_load_string($fetcher_result->getRaw());
      dpm($feed_xml, "feed_xml");
      $feed = common_syndication_parser_parse($fetcher_result->getRaw());
      dpm($feed, "feed");
      dpm($source, "source");
      
      $result = new FeedsParserResult();
      $result->title = $feed['title'];
      $result->description = $feed['description'];
      $result->link = $feed['link'];
      if (is_array($feed['items'])) {
         foreach ($feed['items'] as $item) {
            dpm($item, "feed item in parser");
            $json = $item['description'];
            $data = json_decode($json);
            $item = array_merge($item, (array)$data);
            $item['description'] = ''; //description gets mapped to Full Text of the biblio node so leave it blank
            $result->items[] = $item;
            $source->zoteroChildren[$item->itemType][$item->timestamp] = $item;
            $this->addZoteroChildren($source, $item);
         }
      }
      return $result;
   }
   
   /**
   * Return mapping sources.
   * Zotero field names for sources are found here: http://www.zotero.org/support/dev/client_coding/javascript_api/search_fields
   */
   public function getMappingSources() {
      $sources = array(
         'url' => array(
            'name' => t('zotero:url'),
            'description' => t(''),
            ),
         'rights' => array(
            'name' => t('zotero:rights'),
            'description' => t(''),
            ),
         'series' => array(
            'name' => t('zotero:series'),
            'description' => t(''),
            ),
         'volume' => array(
            'name' => t('zotero:volume'),
            'description' => t(''),
            ),
         'issue' => array(
            'name' => t('zotero:issue'),
            'description' => t(''),
            ),
         'edition' => array(
            'name' => t('zotero:edition'),
            'description' => t(''),
            ),
         'place' => array(
            'name' => t('zotero:place'),
            'description' => t(''),
            ),
         'publisher' => array(
            'name' => t('zotero:publisher'),
            'description' => t(''),
            ),
         'pages' => array(
            'name' => t('zotero:pages'),
            'description' => t(''),
            ),
         'ISBN' => array(
            'name' => t('zotero:ISBN'),
            'description' => t(''),
            ),
         'publicationTitle' => array(
            'name' => t('zotero:publicationTitle'),
            'description' => t(''),
            ),
         'ISSN' => array(
            'name' => t('zotero:ISSN'),
            'description' => t(''),
            ),
         'date' => array(
            'name' => t('zotero:date'),
            'description' => t(''),
            ),
         'section' => array(
            'name' => t('zotero:section'),
            'description' => t(''),
            ),
         'callNumber' => array(
            'name' => t('zotero:callNumber'),
            'description' => t(''),
            ),
         'archiveLocation' => array(
            'name' => t('zotero:archiveLocation'),
            'description' => t(''),
            ),
         'distributor' => array(
            'name' => t('zotero:distributor'),
            'description' => t(''),
            ),
         'extra' => array(
            'name' => t('zotero:extra'),
            'description' => t(''),
            ),
         'journalAbbreviation' => array(
            'name' => t('zotero:journalAbbreviation'),
            'description' => t(''),
            ),
         'DOI' => array(
            'name' => t('zotero:DOI'),
            'description' => t(''),
            ),
         'accessDate' => array(
            'name' => t('zotero:accessDate'),
            'description' => t(''),
            ),
         'seriesTitle' => array(
            'name' => t('zotero:seriesTitle'),
            'description' => t(''),
            ),
         'seriesText' => array(
            'name' => t('zotero:seriesText'),
            'description' => t(''),
            ),
         'seriesNumber' => array(
            'name' => t('zotero:seriesNumber'),
            'description' => t(''),
            ),
         'institution' => array(
            'name' => t('zotero:institution'),
            'description' => t(''),
            ),
         'reportType' => array(
            'name' => t('zotero:reportType'),
            'description' => t(''),
            ),
         'code' => array(
            'name' => t('zotero:code'),
            'description' => t(''),
            ),
         'session' => array(
            'name' => t('zotero:session'),
            'description' => t(''),
            ),
         'legislativeBody' => array(
            'name' => t('zotero:legislativeBody'),
            'description' => t(''),
            ),
         'history' => array(
            'name' => t('zotero:history'),
            'description' => t(''),
            ),
         'reporter' => array(
            'name' => t('zotero:reporter'),
            'description' => t(''),
            ),
         'court' => array(
            'name' => t('zotero:court'),
            'description' => t(''),
            ),
         'numberOfVolumes' => array(
            'name' => t('zotero:numberOfVolumes'),
            'description' => t(''),
            ),
         'committee' => array(
            'name' => t('zotero:committee'),
            'description' => t(''),
            ),
         'assignee' => array(
            'name' => t('zotero:assignee'),
            'description' => t(''),
            ),
         'patentNumber' => array(
            'name' => t('zotero:patentNumber'),
            'description' => t(''),
            ),
         'priorityNumbers' => array(
            'name' => t('zotero:priorityNumbers'),
            'description' => t(''),
            ),
         'issueDate' => array(
            'name' => t('zotero:issueDate'),
            'description' => t(''),
            ),
         'references' => array(
            'name' => t('zotero:references'),
            'description' => t(''),
            ),
         'legalStatus' => array(
            'name' => t('zotero:legalStatus'),
            'description' => t(''),
            ),
         'codeNumber' => array(
            'name' => t('zotero:codeNumber'),
            'description' => t(''),
            ),
         'artworkMedium' => array(
            'name' => t('zotero:artworkMedium'),
            'description' => t(''),
            ),
         'number' => array(
            'name' => t('zotero:number'),
            'description' => t(''),
            ),
         'artworkSize' => array(
            'name' => t('zotero:artworkSize'),
            'description' => t(''),
            ),
         'libraryCatalog' => array(
            'name' => t('zotero:libraryCatalog'),
            'description' => t(''),
            ),
         'videoRecordingFormat' => array(
            'name' => t('zotero:videoRecordingFormat'),
            'description' => t(''),
            ),
         'interviewMedium' => array(
            'name' => t('zotero:interviewMedium'),
            'description' => t(''),
            ),
         'letterType' => array(
            'name' => t('zotero:letterType'),
            'description' => t(''),
            ),
         'manuscriptType' => array(
            'name' => t('zotero:manuscriptType'),
            'description' => t(''),
            ),
         'mapType' => array(
            'name' => t('zotero:mapType'),
            'description' => t(''),
            ),
         'scale' => array(
            'name' => t('zotero:scale'),
            'description' => t(''),
            ),
         'thesisType' => array(
            'name' => t('zotero:thesisType'),
            'description' => t(''),
            ),
         'websiteType' => array(
            'name' => t('zotero:websiteType'),
            'description' => t(''),
            ),
         'audioRecordingFormat' => array(
            'name' => t('zotero:audioRecordingFormat'),
            'description' => t(''),
            ),
         'label' => array(
            'name' => t('zotero:label'),
            'description' => t(''),
            ),
         'presentationType' => array(
            'name' => t('zotero:presentationType'),
            'description' => t(''),
            ),
         'meetingName' => array(
            'name' => t('zotero:meetingName'),
            'description' => t(''),
            ),
         'studio' => array(
            'name' => t('zotero:studio'),
            'description' => t(''),
            ),
         'runningTime' => array(
            'name' => t('zotero:runningTime'),
            'description' => t(''),
            ),
         'network' => array(
            'name' => t('zotero:network'),
            'description' => t(''),
            ),
         'postType' => array(
            'name' => t('zotero:postType'),
            'description' => t(''),
            ),
         'audioFileType' => array(
            'name' => t('zotero:audioFileType'),
            'description' => t(''),
            ),
         'version' => array(
            'name' => t('zotero:version'),
            'description' => t(''),
            ),
         'system' => array(
            'name' => t('zotero:system'),
            'description' => t(''),
            ),
         'company' => array(
            'name' => t('zotero:company'),
            'description' => t(''),
            ),
         'conferenceName' => array(
            'name' => t('zotero:conferenceName'),
            'description' => t(''),
            ),
         'encyclopediaTitle' => array(
            'name' => t('zotero:encyclopediaTitle'),
            'description' => t(''),
            ),
         'dictionaryTitle' => array(
            'name' => t('zotero:dictionaryTitle'),
            'description' => t(''),
            ),
         'language' => array(
            'name' => t('zotero:language'),
            'description' => t(''),
            ),
         'programmingLanguage' => array(
            'name' => t('zotero:programmingLanguage'),
            'description' => t(''),
            ),
         'university' => array(
            'name' => t('zotero:university'),
            'description' => t(''),
            ),
         'abstractNote' => array(
            'name' => t('zotero:abstractNote'),
            'description' => t(''),
            ),
         'websiteTitle' => array(
            'name' => t('zotero:websiteTitle'),
            'description' => t(''),
            ),
         'reportNumber' => array(
            'name' => t('zotero:reportNumber'),
            'description' => t(''),
            ),
         'billNumber' => array(
            'name' => t('zotero:billNumber'),
            'description' => t(''),
            ),
         'codeVolume' => array(
            'name' => t('zotero:codeVolume'),
            'description' => t(''),
            ),
         'codePages' => array(
            'name' => t('zotero:codePages'),
            'description' => t(''),
            ),
         'dateDecided' => array(
            'name' => t('zotero:dateDecided'),
            'description' => t(''),
            ),
         'reporterVolume' => array(
            'name' => t('zotero:reporterVolume'),
            'description' => t(''),
            ),
         'firstPage' => array(
            'name' => t('zotero:firstPage'),
            'description' => t(''),
            ),
         'documentNumber' => array(
            'name' => t('zotero:documentNumber'),
            'description' => t(''),
            ),
         'dateEnacted' => array(
            'name' => t('zotero:dateEnacted'),
            'description' => t(''),
            ),
         'publicLawNumber' => array(
            'name' => t('zotero:publicLawNumber'),
            'description' => t(''),
            ),
         'country' => array(
            'name' => t('zotero:country'),
            'description' => t(''),
            ),
         'applicationNumber' => array(
            'name' => t('zotero:applicationNumber'),
            'description' => t(''),
            ),
         'forumTitle' => array(
            'name' => t('zotero:forumTitle'),
            'description' => t(''),
            ),
         'episodeNumber' => array(
            'name' => t('zotero:episodeNumber'),
            'description' => t(''),
            ),
         'blogTitle' => array(
            'name' => t('zotero:blogTitle'),
            'description' => t(''),
            ),
         'type' => array(
            'name' => t('zotero:type'),
            'description' => t(''),
            ),
         'medium' => array(
            'name' => t('zotero:medium'),
            'description' => t(''),
            ),
         'title' => array(
            'name' => t('zotero:title'),
            'description' => t(''),
            ),
         'tags' => array(
            'name' => t('zotero:tags'),
            'description' => t(''),
            ),
         'caseName' => array(
            'name' => t('zotero:caseName'),
            'description' => t(''),
            ),
         'nameOfAct' => array(
            'name' => t('zotero:nameOfAct'),
            'description' => t(''),
            ),
         'subject' => array(
            'name' => t('zotero:subject'),
            'description' => t(''),
            ),
         'proceedingsTitle' => array(
            'name' => t('zotero:proceedingsTitle'),
            'description' => t(''),
            ),
         'bookTitle' => array(
            'name' => t('zotero:bookTitle'),
            'description' => t(''),
            ),
         'shortTitle' => array(
            'name' => t('zotero:shortTitle'),
            'description' => t(''),
            ),
         'docketNumber' => array(
            'name' => t('zotero:docketNumber'),
            'description' => t(''),
            ),
         'numPages' => array(
            'name' => t('zotero:numPages'),
            'description' => t(''),
            ),
         'programTitle' => array(
            'name' => t('zotero:programTitle'),
            'description' => t(''),
            ),
         'issuingAuthority' => array(
            'name' => t('zotero:issuingAuthority'),
            'description' => t(''),
            ),
         'filingDate' => array(
            'name' => t('zotero:filingDate'),
            'description' => t(''),
            ),
         'genre' => array(
            'name' => t('zotero:genre'),
            'description' => t(''),
            ),
         'archive' => array(
            'name' => t('zotero:archive'),
            'description' => t(''),
            ),
         'note' => array(
            'name' => t('zotero:note'),
            'description' => t('This  field contains all notes for an item merged into a single text field.'),
            ),
         ) + parent::getMappingSources();
         return $sources;
   }
   
   private function addZoteroChildren( FeedsSource $source, &$item ) {
      // get the child items: notes, attachments, and what else?
      /** @TODO somehow keep notes metadata, author, tags, etc. **/
      $childless_types = array( 'note',);
      if ( ! in_array( $item->itemType,  self::getChildlessZoteroTypes() ) ) {
         $conf = $source->getConfig();
         dpm($conf, "source conf");
         dpm($this->getConfig(), "this->getConfig()");
         
         $feed_url = $conf['FeedsZoteroFetcher']['source'];
         $q_pos = strpos( $feed_url, '?');
         $url = substr($feed_url, 0, $q_pos);  // api base url
         
         $guid = $item['guid']; // e.g. http://zotero.org/groups/travis_testing/items/BED4T6FT
         $z_pos = strrpos( $guid, '/')+1;
         $zid = substr($guid , $z_pos , strlen($guid)); 
         $child_service_url = $url . "/$zid/children?content=json"; // child items url
        dpm($item, "result->current_item");
        dpm($url, "url");
        dpm($guid, "guid");
         dpm($item, "item");
         dpm($child_service_url, "child_service_url");
         
         $doc = file_get_contents($child_service_url); /** @TODO use authentication if in Fetcher **/ 
         $xml = simplexml_load_string($doc);
         foreach ( $xml->entry as $entry ) {
            $child = json_decode( (string) $entry->content );
            // for this to work, go to admin/config/content/biblio/fields and in Common fields set biblio_notes to common
            switch ( $child->itemType ) {
            case 'note':
               $item->note .= strip_tags($child->note) . PHP_EOL; // Concatenate multiple notes into one value
               $item->notes[] = $source->zoteroChildren['note'][$child->timestamp]; // This is here so the processor can do something more exciting than concat note text.
            }
         }
      }
   }
   
   static function getChildlessZoteroTypes() {
      return array('note', 'attachment');
   }
}
