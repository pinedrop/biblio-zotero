<?php

/**
* @file
* Class definition of FeedsZoteroProcessor.
*/

// Create or delete FEEDS_NODE_BATCH_SIZE at a time.
define('FEEDS_NODE_BATCH_SIZE', 50);

// Deprecated. Use FEEDS_SKIPE_EXISTING, FEEDS_REPLACE_EXISTNG,
// FEEDS_UPDATE_EXISTING instead.
define('FEEDS_NODE_SKIP_EXISTING', 0);
define('FEEDS_NODE_REPLACE_EXISTING', 1);
define('FEEDS_NODE_UPDATE_EXISTING', 2);

module_load_include('inc', 'feeds', 'plugins/FeedsNodeProcessor');

/**
* Creates nodes from feed items.
* This is a modified version of FeedsNodeProcessor
*/
class FeedsZoteroProcessor extends FeedsNodeProcessor {
   
   /**
   * Override parent::configDefaults().
   */
   public function configDefaults() {
      $config = parent::configDefaults();
      $config['sync_tags'] = false;
      $config['tags_field'] = null;
      $config['tags_vocab'] = null;
      $config['notes_field'] = 'biblio_notes';
      return $config;
   }
   
   /**
   * Override parent::configForm().
   */
   public function configForm(&$form_state) {
      $form = parent::configForm($form_state);
      dpm($this->config, "this->config");
      
      /* $form['notes_field'] = array(
      '#type' => 'select',
      '#title' => t('Biblio field  for zotero notes?'),
      '#description' => t('Zotero notes connot be mapped using the Mappings form. This '),
      '#options' => array(1 => t('yes'), 0 => t('no')),
      '#default_value' => $this->config['sync_tags'],
      ); */
      
      // get tags vocabs for content type
      $content_type = $this->config['content_type'];
      $fields=field_info_instances("node",$content_type);
      $taxo_field_options = array();
      foreach ($fields as $field_machine_name => $field) {
         $taxo_field_options[$field_machine_name] = $field['label'];
      }
      
      if ( ! empty( $taxo_field_options)) {
         $form['sync_tags'] = array(
            '#type' => 'radios',
            '#title' => t('Sync zotero tags to biblio nodes?'),
            '#options' => array(1 => t('yes'), 0 => t('no')),
            '#default_value' => $this->config['sync_tags'],
            );
         
         $warning = t('Important - to sync tags for this feed, you need at at least one taxonomy vocabulary configured and a term reference field in your content type: ') . l(t('taxonomy settings'), 'admin/content/taxonomy');
         $desc = empty($taxo_field_options) ? sprintf('<span class=\'warning\'>%s</span>', $warning) : '';
         $form['tags_field'] = array(
            '#type' => 'select',
            '#title' => t('Select a field in the @type content type to use for zotero tags.', array('@type' => $content_type)),
            '#description' => $desc,
            '#options' => $taxo_field_options,
            '#default_value' => $this->config['tags_field'],
            );
         
         $values['tags_vocab'] = array(
            '#type' => 'textfield',
            '#title' => t('Vocab Machine Name [not editable]'),
            '#size' => 60, 
            // '#disabled' => TRUE,
            '#default_value' => $this->config['tags_vocab'] ? $this->config['tags_vocab'] : '',
            '#description' => $this->config['tags_vocab'] ? '' : t('<span class="warning">No taxonomy term reference field has been submitted yet</span>'),
            );
      }
      else {
         $form['no_vocabs'] = array(
            '#markup' => t('<p class="warning">Please configure at least one !taxo_link to import zotero tags. </p>', array( '!taxo_link' => l(t('taxonomy vocabulary'), 'admin/structure/taxonomy')))
            );
      }
      return $form;
   }
   
   /**
   * Override parent::configFormValidate().
   */
   public function configFormValidate(&$values) {
      parent::configFormValidate($values);
      if ( !empty($values['sync_tags']) && empty($values['tags_field'])) {
         $warning = t('Important - to sync tags for this feed, you need at at least one taxonomy vocabulary configured and a term reference field in your content type:  !taxo_admin', array( '!taxo_admin' => l(t('taxonomy settings'), 'admin/content/taxonomy') ) );
         form_set_error('tags_field', $warning);
      }
      
      if ( !empty($values['tags_field'])) {
         // get the taxo machine name from the field info
         $taxo_field_name = $this->config['tags_field'];
         $taxo_field_info = field_info_field($values['tags_field']);
         if ( $taxo_field_info['settings']['allowed_values'][0]['vocabulary'] ) {
            $vocab_machine_name = $taxo_field_info['settings']['allowed_values'][0]['vocabulary']; 
            $values['tags_vocab'] = $vocab_machine_name;
         }
         else {
            $warning = t('');
            form_set_error('tags_field', $warning);
         }
      }
   }
   
   /**
   * Reschedule if expiry time changes.
   */
   public function configFormSubmit(&$values) {
      parent::configFormSubmit($values);
   }
   
   
   /**
   * Return available mapping targets.
   * target names are from MySql Db:  DESCRIBE biblio_fields
   */
   public function getMappingTargets() {
      $targets = array(
         /*          'title' => array(
         'name' => t('Title'),
         'description' => t('The title of the node.'),
         ),
         'nid' => array(
         'name' => t('Node ID'),
         'description' => t('The nid of the node. NOTE: use this feature with care, node ids are usually assigned by Drupal.'),
         'optional_unique' => TRUE,
         ),
         'uid' => array(
         'name' => t('User ID'),
         'description' => t('The Drupal user ID of the node author.'),
         ),
         'status' => array(
         'name' => t('Published status'),
         'description' => t('Whether a node is published or not. 1 stands for published, 0 for not published.'),
         ),
         'created' => array(
         'name' => t('Published date'),
         'description' => t('The UNIX time when a node has been published.'),
         ),
         'url' => array(
         'name' => t('URL'),
         'description' => t('The external URL of the node. E. g. the feed item URL in the case of a syndication feed. May be unique.'),
         'optional_unique' => TRUE,
         ),
         'guid' => array(
         'name' => t('GUID'),
         'description' => t('The external GUID of the node. E. g. the feed item GUID in the case of a syndication feed. May be unique.'),
         'optional_unique' => TRUE,
         ), */
         'biblio_number' => array(
            'name' => t('biblio:number'),
            'description' => '',
            ),
         'biblio_other_number' => array(
            'name' => t('biblio:other number'),
            'description' => '',
            ),
         'biblio_sort_title' => array(
            'name' => t('biblio:sort title'),
            'description' => 'A normalized version of the title, used for sorting on titles. (only first 64 characters saved)',
            ),
         'biblio_secondary_title' => array(
            'name' => t('biblio:secondary title'),
            'description' => '',
            ),
         'biblio_tertiary_title' => array(
            'name' => t('biblio:tertiary title'),
            'description' => '',
            ),
         'biblio_edition' => array(
            'name' => t('biblio:edition'),
            'description' => '',
            ),
         'biblio_publisher' => array(
            'name' => t('biblio:publisher'),
            'description' => '',
            ),
         'biblio_place_published' => array(
            'name' => t('biblio:place published'),
            'description' => '',
            ),
         'biblio_year' => array(
            'name' => 'biblio:year',
            'description' => '',
            //       'callback' => 'biblio_zotero_set_biblio_field',
            ),
         'biblio_volume' => array(
            'name' => t('biblio:volume'),
            'description' => '',
            //       'callback' => 'biblio_zotero_set_biblio_field',
            ),
         'biblio_pages' => array(
            'name' => t('biblio:pages'),
            'description' => '',
            ),
         'biblio_date' => array(
            'name' => t('biblio:date'),
            'description' => '',
            ),
         'biblio_isbn' => array(
            'name' => t('biblio:date'),
            'description' => '',
            ),
         'biblio_lang' => array(
            'name' => t('biblio:lang'),
            'description' => '',
            ),
         'biblio_abst_e' => array(
            'name' => t('biblio:abst e'),
            'description' => '',
            ),
         'biblio_abst_f' => array(
            'name' => t('biblio:abst f'),
            'description' => '',
            ),
         'biblio_url' => array(
            'name' => t('biblio:url'),
            'description' => '',
            ),
         'biblio_issue' => array(
            'name' => t('biblio:issue'),
            'description' => '',
            ),
         'biblio_type_of_work' => array(
            'name' => t('biblio:type of work'),
            'description' => '',
            ),
         'biblio_accession_number' => array(
            'name' => t('biblio:accession number'),
            'description' => '',
            ),
         'biblio_call_number' => array(
            'name' => t('biblio:call number'),
            'description' => '',
            ),
         'biblio_notes' => array(
            'name' => t('biblio:notes'),
            'description' => '',
            ),
         'biblio_custom1' => array(
            'name' => t('biblio:custom1'),
            'description' => '',
            ),
         'biblio_custom2' => array(
            'name' => t('biblio:custom2'),
            'description' => '',
            ),
         'biblio_custom3' => array(
            'name' => t('biblio:custom3'),
            'description' => '',
            ),
         'biblio_custom4' => array(
            'name' => t('biblio:custom4'),
            'description' => '',
            ),
         'biblio_custom5' => array(
            'name' => t('biblio:custom5'),
            'description' => '',
            ),
         'biblio_custom6' => array(
            'name' => t('biblio:custom6'),
            'description' => '',
            ),
         'biblio_custom7' => array(
            'name' => t('biblio:custom7'),
            'description' => '',
            ),
         'biblio_research_notes' => array(
            'name' => t('biblio:research notes'),
            'description' => '',
            ),
         'biblio_number_of_volumes' => array(
            'name' => t('biblio:number of volumes'),
            'description' => '',
            ),
         'biblio_short_title' => array(
            'name' => t('biblio:short title'),
            'description' => '',
            ),
         'biblio_alternate_title' => array(
            'name' => t('biblio:alternate title'),
            'description' => '',
            ),
         'biblio_original_publication' => array(
            'name' => t('biblio:original publication'),
            'description' => '',
            ),
         'biblio_reprint_edition' => array(
            'name' => t('biblio:reprint edition'),
            'description' => '',
            ),
         'biblio_translated_title' => array(
            'name' => t('biblio:translated title'),
            'description' => '',
            ),
         'biblio_section' => array(
            'name' => t('biblio:section'),
            'description' => '',
            ),
         'biblio_citekey' => array(
            'name' => t('biblio:citekey'),
            'description' => '',
            ),
         'biblio_coins' => array(
            'name' => t('biblio:coins'),
            'description' => '',
            ),
         'biblio_doi' => array(
            'name' => t('biblio:doi'),
            'description' => '',
            ),
         'biblio_issn' => array(
            'name' => t('biblio:issn'),
            'description' => '',
            ),
         'biblio_auth_address' => array(
            'name' => t('biblio:auth address'),
            'description' => '',
            ),
         'biblio_remote_db_name' => array(
            'name' => t('biblio:remote db name'),
            'description' => '',
            ),
         'biblio_remote_db_provider' => array(
            'name' => t('biblio:remote db provider'),
            'description' => '',
            ),
         'biblio_label' => array(
            'name' => t('biblio:label'),
            'description' => '',
            ),
         'biblio_access_date' => array(
            'name' => t('biblio:access date'),
            'description' => '',
            ),
         'biblio_refereed' => array(
            'name' => t('biblio:refereed'),
            'description' => '',
            ),
         'biblio_formats' => array(
            'name' => t('biblio:formats'),
            'description' => '',
            ),
         ) + parent::getMappingTargets();
         
         // Let other modules expose mapping targets.
         /** @TODO evaluate if these targets should be added via the biblio_zotero module **/
         self::loadMappers();
         drupal_alter('feeds_node_processor_targets', $targets, $this->config['content_type']);
         
         return $targets;
   }
   
   public function process(FeedsSource $source, FeedsParserResult $parser_result) {
      /** @TODO comment this ZoteroBiblioMap dependency out and try to import; then fix and get rid of the dependency **/
      module_load_include('inc', 'biblio_zotero', 'ZoteroBiblioMap');
      parent::process($source, $parser_result);
   }
   
   
   /**
   * Implementation of map function
   * The zotero items are all in  $result->current_item
   **/
   protected function map(FeedsSource $source, FeedsParserResult $result, $target_item = NULL) {
      
      /* FIX THIS BY LOOKING INTO THE ACTUAL MAPPING
      if ($result->current_item) {
      $manuallyMappedItems = array('guid', 'itemType', 'tags', 'creators institution');
      $bib2Zot = ZoteroBiblioMap::getBiblioToZoteroFields();
      foreach ( $result->current_item as $zot_key => $zot_value ) {
      if ( ! in_array($zot_key, $bib2Zot) && ! in_array($zot_key, $manuallyMappedItems)  && ! empty($zot_value)) {
      $msg = t( "Zotero field, '@zot_key', was not found in the mappings from biblio to zotero fields. The value for this item was '@zot_value'", array("@zot_key" => $zot_key, "@zot_value" => $zot_value) );
      drupal_set_message($msg); 
      }
      }
      } */
      // map the basics
      parent::map($source, $result, $target_item);
      
      // set notes and ??? what-else? to not save
      if ($result->current_item['itemType'] == 'note') {
         $type=$result->current_item['itemType'];
         dpm("Stop Processing item of type: '$type'");
         // skip saving for this item: see FeedsProcessor::process
         $target_item->feeds_item->skip = TRUE;
         return;
      }
      
      // map  zotero fields
      $parser = feeds_importer($this->id)->parser; // $batch's fields are protected so we have to get them from the parser
      
      $tags = $parser->getSourceElement($source, $result, 'tags');
      $tags = is_array($tags) ? $tags : array();
      
      $creators = $parser->getSourceElement($source, $result, 'creators');
      $creators = is_array($creators) ? $creators : array();
      $institution = $parser->getSourceElement($source, $result, 'institution');
      
      $itemType = $parser->getSourceElement($source, $result, 'itemType');
      $target_item->biblio_type = ZoteroBiblioMap::zoteroToBiblioType($itemType);
      
      // map the tags
      if ($this->config['tags_field']) {
         dpm( "taggins");
         
         foreach ($tags as $t) {
            dpm($t, "a t tag");
            $term =  taxonomy_get_term_by_name($t->tag, $this->config['tags_vocab']);
            // field_tags (Array, 1 element)
            //    und (Array, 3 elements)
            //    0 (Array, 7 elements)
            //       tid (String, 1 characters ) 1
            //       vid (String, 1 characters ) 1
            //       name (String, 4 characters ) tag1
            //       description (NULL)
            //       format (NULL)
            //       weight (String, 1 characters ) 0
            //       vocabulary_machine_name (String, 4 characters ) tags
         }
         $target_item->taxonomy['tags'][$this->config['tags_vocab_id']] = implode(",", $tagStrings);
      }
      
      // map the creators
      foreach ($creators as $creator) {
         if($creator->creatorType == 'author') {
				$target_item->biblio_contributors[ZoteroBiblioMap::BIBLIO_AUTHOR][] = array('name' => "$creator->lastName, $creator->firstName");
			}
		}
		if(! empty($institution) ) {
			$target_item->biblio_contributors[ZoteroBiblioMap::BIBLIO_CORPORATE_AUTHOR][] = array('name' => $institution);
		}
		
		
		// add the group
		/** @TODO in D7 this is group_audience **/
		if ( module_exists('og') ) {
		   $feed_nid = $batch->feed_nid;
		   $feed = node_load($feed_nid);
		   $target_item->group_audience = $feed->group_audience;
		   //$target_item->og_groups_both = $feed->og_groups_both;
		}
		dpm($target_item, "target_item");
   }
   
   /**
   * Override setTargetElement to operate on a target item that is a node.
   * $target_element is the field name (e.g. title, guid, biblio_abstract, biblio_call_number, etc
   */
   public function setTargetElement(FeedsSource $source, &$target_node, $target_element, $value) {
      // dpm($target_element, "target_element");
      // dpm($value, "value");
      
      parent::setTargetElement($source, $target_node, $target_element, $value);
      
      /* $biblioToZoteroFields = ZoteroBiblioMap::getBiblioToZoteroFields();
      if (in_array($target_element, array_keys($biblioToZoteroFields))) {
      $target_node->$target_element = $value;
      } */
   }
   
   /**
   * Override parent::entitySave
   * Some zotero item types are actually children. like notes, attachments, etc. In thoses cases do not save
   */
   /*    public function entitySave($entity) {
   // If nid is set and a node with that id doesn't exist, flag as new.
   if (!empty($entity->nid) && !node_load($entity->nid)) {
   $entity->is_new = TRUE;
   }
   dpm($entity->do_not_save ? "entity->DO_NOT_SAVE" : "entity YES SAVE");
   if ( empty( $entity->do_not_save) ) {
   dpm("SAVING ENTITY");
   node_save($entity);
   }
   } */
}
