<?php

/**
* Class definition for Zotero Parser.
*
* Parses RSS and Atom feeds.
*/
class FeedsZoteroParser extends FeedsSyndicationParser {
   /**
   * Override parent::configDefaults().
   */
   public function configDefaults() {
      $config = parent::configDefaults();
      $config['refresh_zotero_item_type_fields'] = false;
      return $config;
   }
   
   /**
   * Override parent::configForm().
   */
   public function configForm(&$form_state) {
      $form = parent::configForm($form_state);
      
      $form['refresh_zotero_item_type_fields'] = array(
         '#type' => 'radios',
         '#title' => t('Refresh the item type fields?'),
         '#options' => array(1 => t('yes'), 0 => t('no')),
         '#default_value' => $this->config['refresh_zotero_item_type_fields'],
         '#description' => t('Refresh the item type fields every time the mappings are requested [e.g. on the mappings settings page]. Zotero occasionally changes the fields available for each item type so it is necessary to update the mapping sources. This will make the mappings page load slowly so it is best to uncheck this after you\'ve updated.'),
         
         );
      return $form;
   }
   
   /**
   * Override parent::configFormValidate().
   */
   public function configFormValidate(&$values) {
   }
   
   /**
   * Implementation of FeedsParser::parse().
   */
   public function parse(FeedsSource $source, FeedsFetcherResult $fetcher_result) {
      feeds_include_library('common_syndication_parser.inc', 'common_syndication_parser');
      
      $feed_raw_xml = $fetcher_result->getRaw();
      //dpm($feed_raw_xml, "feed_raw_xml");
      $feed_simple_xml = simplexml_load_string($feed_raw_xml);
      $namespaces = $feed_simple_xml->getNameSpaces(true);
      
      $result = new FeedsParserResult();
      $result->title = (string)$feed_simple_xml->title;
      $result->link = (string)array_shift($feed_simple_xml->link);
      
      foreach ($feed_simple_xml->entry as $entry) {
         $item = array();
         
         $json = (string)$entry->content;
         $data = json_decode($json);
         $item = array_merge($item, (array)$data);
         self::addItemTypePrefixes($item);
         
         $item['guid'] = (string)$entry->id;
         $item['published'] = (string)$entry->published;
         $item['updated'] = (string)$entry->updated;
         $item['description'] = ''; //description gets mapped to Full Text of the biblio node so leave it blank
         
         $zapi = $entry->children($namespaces['zapi']); 
         $item['key'] = (string)$zapi->key;
         $item['itemType'] = (string)$zapi->itemType;
         $item['creatorSummary'] = (string)$zapi->creatorSummary;
         $item['year'] = (string)$zapi->year;
         $item['numChildren'] = (string)$zapi->numChildren;
         $item['numTags'] = (string)$zapi->numTags;
         $this->addZoteroChildren($source, $item);   
         $result->items[] = $item;
      }
      return $result;
   }
   
   /**
   * Return mapping sources.
   * Zotero field names for sources are found here: http://www.zotero.org/support/dev/client_coding/javascript_api/search_fields
   */
   public function getMappingSources() {
      module_load_include('inc', 'biblio_zotero', 'zotero');
      $refresh = $this->config['refresh_zotero_item_type_fields'];
      $zoteroItemStructure = Zotero::getItemTypeStructure($refresh);
         dd($zoteroItemStructure, "zoteroItemStructure");
      $sources = array();
      foreach ( $zoteroItemStructure as  $key => $zotero_type ) {
         $item_type = $key;
         $item_type_label = $zotero_type->localized;
         foreach ( $zotero_type->fields as $key => $field) {       
            $source = "$item_type:$key";
            $sources[$source] = array(
               'name' => t("Zotero - $item_type_label - $field->localized"),
               );
         }
      }
      foreach ( $zoteroItemCreators as  $key => $zotero_type ) {
         $item_type = $key;
         $item_type_label = $zotero_type->localized;
         foreach ( $zotero_type->fields as $key => $field) {       
            $source = "$item_type:$key";
            $sources[$source] = array(
               'name' => t("Zotero - $item_type_label - $field->localized"),
               );
         }
      }

      $sources += parent::getMappingSources();
      return $sources;
   }
   
   private function addZoteroChildren( FeedsSource $source, &$item ) {
      // get the child items: notes, attachments, and what else?
      /** @TODO somehow keep notes metadata, author, tags, etc. **/
      if ( $item['numChildren'] > 0 ) {
         $conf = $source->getConfig();
         $feed_url = $conf['FeedsZoteroFetcher']['source'];
         $q_pos = strpos( $feed_url, '?');
         $url = substr($feed_url, 0, $q_pos);  // api base url
         $zapiKey = $item['key'];
         $child_service_url = $url . "/$zapiKey/children?content=json"; // child items url
         if ( ! empty( $conf['FeedsZoteroFetcher']['api_key'] ) ) {
            $child_service_url .= '&key=' . $conf['FeedsZoteroFetcher']['api_key'];
         }
         
         $xml_str = file_get_contents($child_service_url); /** @TODO use authentication if in Fetcher **/ 
         $xml = simplexml_load_string($xml_str);
         $namespaces = $xml->getNameSpaces(true);
         
         foreach ( $xml->entry as $entry ) {
            $zapi = $entry->children($namespaces['zapi']); 
            $child = json_decode( (string) $entry->content );
            $child->key = (string)$zapi->key;
            $child->itemType = (string)$zapi->itemType;
            $child->author = (string)$zapi->author->name;
            // for this to work, go to admin/config/content/biblio/fields and in Common fields set biblio_notes to common
            switch ( $child->itemType ) {
            case 'note':
               $note = $child->note . PHP_EOL . " - " . $child->author;
               // Concatenate multiple notes into one value; this 'note' field is exposed as a mapping source
               $item['note'] .= strip_tags($note) . PHP_EOL; 
               // This is here so the processor can do something more exciting than concat note text. 
               //@TODO expose 'notes' this with a proper source callback and target callback
               $item['notes'][] = $child; 
            }
         }
      }
   }
   
   /**
   * Create unique mapping sources for the different zotero types 
   * since they sometimes map to different biblio fields depending on publication type
   */
   function addItemTypePrefixes(&$item) {
      $no_rename = array('itemType', 'guid','tags','creators');
      $item = (array)$item;
      $old_keys = array_keys($item);
      foreach( $old_keys as $old_key) {
         if (in_array($old_key, $no_rename)) {
            continue;
         }
         $new_key = $item['itemType'] . ":" . $old_key;
         $item[$new_key] = $item[$old_key];
         unset($item[$old_key]);
      }
   }
}
