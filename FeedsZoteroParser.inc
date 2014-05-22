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
      
     /* THIS IS HANDY FOR DEBUGGING
     $form['refresh_zotero_item_type_fields'] = array(
         '#type' => 'radios',
         '#title' => t('Refresh the item type fields?'),
         '#options' => array(1 => t('yes'), 0 => t('no')),
         '#default_value' => $this->config['refresh_zotero_item_type_fields'],
         '#description' => t('Refresh the item type fields every time the mappings are requested [e.g. on the mappings settings page]. Zotero occasionally changes the fields available for each item type so it is necessary to update the mapping sources. This will make the mappings page load slowly so it is best to uncheck this after you\'ve updated.'),
         
         ); */
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
      
      $source_config = $source->getConfig();
      $result = new FeedsParserResult();
      $result->collections = $fetcher_result->getCollectionHierarchy();      
      foreach ($fetcher_result->getItemPages() as $collection_key => $pages) { 
        $collection_key = ($collection_key == 'no_collection_items') ? FALSE : $collection_key;
        foreach ($pages as $page) {

          $feed_simple_xml = simplexml_load_string($page);
          $namespaces = $feed_simple_xml->getNameSpaces(true);

          foreach ($feed_simple_xml->entry as $entry) {
            $zapi = $entry->children($namespaces['zapi']); 
            $item_key = (string)$zapi->key;
            $item_type = (string)$zapi->itemType;
            $item = array();
            $json = (string)$entry->content;
            $data = json_decode($json);
            $item = array_merge($item, (array)$data);
            
            //Item may have been processed in another collection, copy collection keys from result if they exist
            $item['collection_keys'] = isset($result->items[$item_key]) ? $result->items[$item_key]['collection_keys'] : array();

            if ($collection_key) {
              $item['collection_keys'][] = $collection_key;
            }
             
            $item['key'] = $item_key;
            $item['guid'] = $item_key;
             
            $item['url'] = (string)$entry->id;
            $item['published'] = (string)$entry->published;
            $item['updated'] = (string)$entry->updated;
            $item['description'] = ''; //description gets mapped to Full Text of the biblio node so leave it blank
            $item['title'] = (string)$entry->title;
            $item['long_title'] = (string)$entry->title;
             
            $item['itemType'] = (string)$zapi->itemType;
            $item['creatorSummary'] = (string)$zapi->creatorSummary;
            $item['year'] = (string)$zapi->year;
            $item['numChildren'] = (string)$zapi->numChildren;
            $item['numTags'] = (string)$zapi->numTags;             
            self::addItemTypePrefixes($item);
            $this->addZoteroChildren($item, $fetcher_result->getChildPages($item_key));   

            $result->items[(string)$zapi->key] = $item;            
          }
        }
      }
      $result->title = $result->title ? $result->title : t('Zotero Feed Importer');
      return $result;
   }
   
   public function addZoteroChildren(&$item, $children) {
     foreach ($children as $child) { 
       $xml = simplexml_load_string($child);
       $namespaces = $xml->getNameSpaces(true);
       
       foreach ($xml->entry as $entry) {
         $child = json_decode( (string) $entry->content );
         $child->author = (string)$entry->author->name;
         $zapi = $entry->children($namespaces['zapi']); 
         $child->key = (string)$zapi->key;
         $child->itemType = (string)$zapi->itemType;
         // for this to work, go to admin/config/content/biblio/fields and in Common fields set biblio_notes to common
         $biggest_note_size = 0; 
         switch ($child->itemType) {
           case 'note':
             $note = $child->note . PHP_EOL . " - " . $child->author;
             // Concatenate multiple notes into one value; this 'note' field is exposed as a mapping source
             $item['note'] .= mysql_real_escape_string(strip_tags($note)) . PHP_EOL; 
             // This is here so the processor can do something more exciting than concat note text. 
             //@TODO expose 'notes' this with a proper source callback and target callback
             $item['notes'][] = $child; 
             break;
           case 'attachment':
             if ($child->url) {
               $item['links'][] = mysql_real_escape_string($child->url);
             }
             elseif ($child->filename) {
               $item['files'][] = mysql_real_escape_string($child->filename);
             }
             break;

           default:
         }
       }
     }
   }
   
   /**
   * Return mapping sources.
   * Zotero field names for sources are found here: http://www.zotero.org/support/dev/client_coding/javascript_api/search_fields
   */
   public function getMappingSources() {
      module_load_include('inc', 'biblio_zotero', 'biblio_zotero');
      $refresh = $this->config['refresh_zotero_item_type_fields'];
      $zoteroItemStructure = BiblioZotero::getItemTypeStructure($refresh);
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
      $sources['creators'] = array(
         'name' => t("Zotero - Creators"),
         'description' => t("Zotero creators vary by item type and the mapping is determined in a callback function provided in biblio_zotero.module"),
         'callback' => 'biblio_zotero_feeds_source_creators',
         );
      $sources['links'] = array(
         'name' => t("Zotero Links"),
         'description' => t("Link attachments from Zotero"),
         );
      $sources['files'] = array(
         'name' => t("Zotero Filenames"),
         'description' => t("Names of attached files from Zotero"),
  //         'callback' => 'link_feeds_set_target',
         );

      $sources += parent::getMappingSources();
      return $sources;
   }
   
   
   /**
   * Create unique mapping sources for the different zotero types 
   * since they sometimes map to different biblio fields depending on publication type
   */
   function addItemTypePrefixes(&$item) {
      $no_rename = array('itemType', 'guid','tags','creators', 'collection_keys', 'files', 'links', 'long_title');
      $item = (array)$item;
      $old_keys = array_keys($item);
      foreach( $old_keys as $old_key) {
         if (in_array($old_key, $no_rename)) {
            continue;
         }
         if ($item['itemType']) {
           $new_key = $item['itemType'] . ":" . $old_key;
           $item[$new_key] = $item[$old_key];
           unset($item[$old_key]);
         }
      }
   }
}
