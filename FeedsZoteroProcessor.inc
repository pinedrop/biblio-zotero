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
            '#title' => t('Select a term reference field in the @type content type to use for zotero tags.', array('@type' => $content_type)),
            '#description' => $desc,
            '#options' => $taxo_field_options,
            '#default_value' => $this->config['tags_field'],
            );
         
         $form['tags_vocab'] = array(
            '#type' => 'textfield',
            '#title' => t('Vocab Machine Name [not editable]'),
            '#size' => 60, 
            '#disabled' => TRUE,
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
//  THESE ARE ACTUALL ALREADY PROVIDED IN biblio.feeds.inc _biblio_feeds_processor_targets_alter()   
   public function getMappingTargets() {
      $targets = array(
         'biblio_number' => array(
            'name' => t('Biblio - Number'),
            ),
         'biblio_other_number' => array(
            'name' => t('Biblio - Other number'),
            ),
         'biblio_sort_title' => array(
            'name' => t('Biblio - Sort title'),
            'description' => 'A normalized version of the title, used for sorting on titles. (only first 64 characters saved)',
            ),
         'biblio_secondary_title' => array(
            'name' => t('Biblio - Secondary title'),
            ),
         'biblio_tertiary_title' => array(
            'name' => t('Biblio - Tertiary title'),
            ),
         'biblio_edition' => array(
            'name' => t('Biblio - Edition'),
            ),
         'biblio_publisher' => array(
            'name' => t('Biblio - Publisher'),
            ),
         'biblio_place_published' => array(
            'name' => t('Biblio - Place published'),
            ),
         'biblio_year' => array(
            'name' => 'Biblio - Year',
            ),
         'biblio_volume' => array(
            'name' => t('Biblio - Volume'),
            ),
         'biblio_pages' => array(
            'name' => t('Biblio - Pages'),
            ),
         'biblio_date' => array(
            'name' => t('Biblio - Date'),
            ),
         'biblio_isbn' => array(
            'name' => t('Biblio - ISBN'),
            ),
         'biblio_lang' => array(
            'name' => t('Biblio - Language'),
            ),
         'biblio_abst_e' => array(
            'name' => t('Biblio - Abstract E'),
            ),
         'biblio_abst_f' => array(
            'name' => t('Biblio - Abstract F'),
            ),
         'biblio_url' => array(
            'name' => t('Biblio - Url'),
            ),
         'biblio_issue' => array(
            'name' => t('Biblio - Issue'),
            ),
         'biblio_type_of_work' => array(
            'name' => t('Biblio - Type of work'),
            ),
         'biblio_accession_number' => array(
            'name' => t('Biblio - Accession number'),
            ),
         'biblio_call_number' => array(
            'name' => t('Biblio - Call number'),
            ),
         'biblio_notes' => array(
            'name' => t('Biblio - Notes'),
            ),
         'biblio_custom1' => array(
            'name' => t('Biblio - Custom 1'),
            ),
         'biblio_custom2' => array(
            'name' => t('Biblio - Custom 2'),
            ),
         'biblio_custom3' => array(
            'name' => t('Biblio - Custom 3'),
            ),
         'biblio_custom4' => array(
            'name' => t('Biblio - Custom 4'),
            ),
         'biblio_custom5' => array(
            'name' => t('Biblio - Custom 5'),
            ),
         'biblio_custom6' => array(
            'name' => t('Biblio - Custom 6'),
            ),
         'biblio_custom7' => array(
            'name' => t('Biblio - Custom 7'),
            ),
         'biblio_research_notes' => array(
            'name' => t('Biblio - Research notes'),
            ),
         'biblio_number_of_volumes' => array(
            'name' => t('Biblio - Number of volumes'),
            ),
         'biblio_short_title' => array(
            'name' => t('Biblio - Short title'),
            ),
         'biblio_alternate_title' => array(
            'name' => t('Biblio - Alternate title'),
            ),
         'biblio_original_publication' => array(
            'name' => t('Biblio - Original publication'),
            ),
         'biblio_reprint_edition' => array(
            'name' => t('Biblio - Reprint edition'),
            ),
         'biblio_translated_title' => array(
            'name' => t('Biblio - Translated title'),
            ),
         'biblio_section' => array(
            'name' => t('Biblio - Section'),
            ),
         'biblio_citekey' => array(
            'name' => t('Biblio - Citekey'),
            ),
         'biblio_coins' => array(
            'name' => t('Biblio - COinS'),
            ),
         'biblio_doi' => array(
            'name' => t('Biblio - DOI'),
            ),
         'biblio_issn' => array(
            'name' => t('Biblio - Issn'),
            ),
         'biblio_auth_address' => array(
            'name' => t('Biblio - Author address'),
            ),
         'biblio_remote_db_name' => array(
            'name' => t('Biblio - Remote db name'),
            ),
         'biblio_remote_db_provider' => array(
            'name' => t('Biblio - Remote db provider'),
            ),
         'biblio_label' => array(
            'name' => t('Biblio - Label'),
            ),
         'biblio_access_date' => array(
            'name' => t('Biblio - Access date'),
            ),
         'biblio_refereed' => array(
            'name' => t('Biblio - Refereed'),
            ),
         'biblio_formats' => array(
            'name' => t('Biblio - Formats'),
            ),
         ) + parent::getMappingTargets();
         
         return $targets;
   }

   public function process(FeedsSource $source, FeedsParserResult $parser_result) {
      /** @TODO comment this ZoteroBiblioMap dependency out and try to import; then fix and get rid of the dependency **/
      module_load_include('inc', 'biblio_zotero', 'ZoteroBiblioMap');
      $source->reportFilePath = file_directory_temp() ."/biblio-zotero-mapping-report-" . time() . ".tab.txt"; 
      file_put_contents($source->reportFilePath, "zotero itemtype\tzotero field name\tbiblio field mapping");
      parent::process($source, $parser_result);
      drupal_set_message( t('Biblio Zotero wrote an import report of mapped and un-mapped fields to @filename', array('@filename' => $source->reportFilePath)));
      /** @TODO look at parent:process to see about UPDATING during import **/
   }
   
   /**
   * Implementation of map function
   * The zotero items are all in  $result->current_item
   **/
   protected function map(FeedsSource $source, FeedsParserResult $result, $target_item = NULL) {
      $zot2bib = $this->getZot2BibMappings();
      
      // Log unmapped fields to watchdog
      $map_report = array();
      foreach ($result->current_item as $key => $val) {
         
         $dont_log = array( 'guid', 'published', 'updated', 'key', 'itemType', 'description', 'creatorSummary','year', 'tags', 'creators');
         if ( in_array($key, $dont_log) || ! in_array($key, array_keys(FeedsZoteroParser::getMappingSources()))) {
            continue;
         }
         
         $target = $zot2bib[$key];
         $val = empty($val) ? 'NO VALUE' : $val;   
         $trunc_val = isset($val) && strlen($val) < 25 ? $val : substr($val,0,25) . "..." ;
         if ( empty($target) ) {
            $msg = t("Source '@key' with value '@val' was NOT MAPPED to any target", array('@key' => $key, '@val' => $trunc_val));
            $mapped = FALSE;
         } else {
            $msg = t("Source '@key' with value '@val' was mapped to target '@target'", array('@key' => $key, '@val' => $trunc_val, '@target' => $target));
            $mapped = TRUE;
         }
         
         if ( isset($msg) ) {
            /** @TODO write the map success/failures to a tab-delim file */
            $keybits = explode(":", $key);
            $it = $keybits[0];
            $fn = $keybits[1];
            $m = $mapped ? "mapped":"unmapped";
            $map_report[$it][$fn] = !empty($target) ? $target : 'UNMAPPED';
            $tab_data = "$it	$fn	$trunc_val	$m"; 
            watchdog('FeedsZoteroProcessor', $msg, NULL, WATCHDOG_NOTICE, $link = NULL);
         }
         unset($msg);
      }
      
      foreach ($map_report as $it => $row) {
         foreach($row as $fn => $m) {
            file_put_contents($source->reportFilePath, "$it\t$fn\t$m" . PHP_EOL, FILE_APPEND);
         }
      }
      
      // Do the mappings
      // parent::map IS OVERRIDDEN below in super_map parent::map($source, $result, $target_item);
      $this->super_map($source, $result, $target_item);
      
      if ($result->current_item['itemType'] == 'note') {
         $type=$result->current_item['itemType'];
         $target_item->feeds_item->skip = TRUE; // skip saving for this item: see FeedsProcessor::process
         return;
      }
      
      // map zotero fields
      $parser = feeds_importer($this->id)->parser; // $batch's fields are protected so we have to get them from the parser
      $itemType = $parser->getSourceElement($source, $result, 'itemType');
      $target_item->biblio_type = BiblioZotero::getZoteroTypeToBiblioType($itemType);
      
      // map the tags 
      /** @TODO figure out how to use mapping targets for this **/
      $tags = $parser->getSourceElement($source, $result, 'tags');
      $tags = is_array($tags) ? $tags : array();
      if ($this->config['tags_field']) {
         foreach ($tags as $t) {
            $term =  taxonomy_get_term_by_name($t->tag, $this->config['tags_vocab']);
            if (  ! empty($term) ) {
               $term = array_pop($term);
            } else {
               $vocab = taxonomy_vocabulary_machine_name_load($this->config['tags_vocab']);
               $term = new stdClass();
               $term->name = $t->tag;
               $term->vid = $vocab->vid;
               taxonomy_term_save($term);
            }
            $target_item->{$this->config['tags_field']}['und'][] = (array)$term; 
         }
      }
      $target_item->taxonomy['tags'][$this->config['tags_vocab_id']] = implode(",", $tagStrings);
      
      
		
		// map the institution
      /** @TODO use a mapping target for this **/
      $institution = $parser->getSourceElement($source, $result, 'institution');
		if(! empty($institution) ) {
		   $contributor = array(
		      'name' => $institution, 
		      'auth_category' => 5, // 1= Primary, 2 = secondary, 3 = tertiary, 4 = Subsidiary, 5 = corporate
		      'auth_type' => 5,);
			$target_item->biblio_contributors[] = $contributor;
		}
		
		// add the group
		if ( module_exists('og') ) {
		   $feed = node_load($target_item->feeds_item->feed_nid);
		   $target_item->group_audience = $feed->group_audience;
		}
   }
   
   /**
   * Execute mapping on an item.
   * This method is lifted in its entirety from FeedsProcessor::map() with one condition added which prevents calling 
   * setTargetElement if the item type doesnt match the item type in the prefix of the the mapping source, e.g. 'book' from book:title
   * This is needed since it appears that if multiple sources are mapped to the same target then that target is overwritten even when 
   * the source item does not contain data keyed to that source. For example, a zotero book type does not contain the key webpage:title so we 
   * need to prevent the null value for webpage:title being applied to the target that we want written with data from book:title
   */
   protected function super_map(FeedsSource $source, FeedsParserResult $result, $target_item = NULL) {
      
      // Static cache $targets as getMappingTargets() may be an expensive method.
      static $sources;
      if (!isset($sources[$this->id])) {
         $sources[$this->id] = feeds_importer($this->id)->parser->getMappingSources();
      }
      static $targets;
      if (!isset($targets[$this->id])) {
         $targets[$this->id] = $this->getMappingTargets();
      }
      $parser = feeds_importer($this->id)->parser;
      if (empty($target_item)) {
         $target_item = array();
      }
      
      // Many mappers add to existing fields rather than replacing them. Hence we
      // need to clear target elements of each item before mapping in case we are
      // mapping on a prepopulated item such as an existing node.
      foreach ($this->config['mappings'] as $mapping) {
         if (isset($targets[$this->id][$mapping['target']]['real_target'])) {
            unset($target_item->{$targets[$this->id][$mapping['target']]['real_target']});
         }
         elseif (isset($target_item->{$mapping['target']})) {
            unset($target_item->{$mapping['target']});
         }
      }
      
      /*
      This is where the actual mapping happens: For every mapping we envoke
      the parser's getSourceElement() method to retrieve the value of the source
      element and pass it to the processor's setTargetElement() to stick it
      on the right place of the target item.
      
      If the mapping specifies a callback method, use the callback instead of
      setTargetElement().
      */
      self::loadMappers();
      foreach ($this->config['mappings'] as $mapping) {
         // Retrieve source element's value from parser.
         if (isset($sources[$this->id][$mapping['source']]) &&
            is_array($sources[$this->id][$mapping['source']]) &&
         isset($sources[$this->id][$mapping['source']]['callback']) &&
         function_exists($sources[$this->id][$mapping['source']]['callback'])) {
         $callback = $sources[$this->id][$mapping['source']]['callback'];
         $value = $callback($source, $result, $mapping['source']);
         }
         else {
            $value = $parser->getSourceElement($source, $result, $mapping['source']);
         }
         
         // Map the source element's value to the target.
         if (isset($targets[$this->id][$mapping['target']]) &&
            is_array($targets[$this->id][$mapping['target']]) &&
         isset($targets[$this->id][$mapping['target']]['callback']) &&
         function_exists($targets[$this->id][$mapping['target']]['callback'])) {
         $callback = $targets[$this->id][$mapping['target']]['callback'];
         $callback($source, $target_item, $mapping['target'], $value);
         }
         else {
            // START modifications to FeedsProcessor::map
            $src_array = explode(":", $mapping['source']);
            $zotero_src_prefix = $src_array[0];
            $ok_to_set_target = sizeof($src_array) == 1 || $zotero_src_prefix == $result->current_item['itemType'];
            if ($ok_to_set_target) {
               $this->setTargetElement($source, $target_item, $mapping['target'], $value); // this is the original code for this block 
            }
            // END modifications
         }
      }
      return $target_item;
   }
   function  getZot2BibMappings(){
      //dd($this->config, "this->config");
      $z2b = &drupal_static(__FUNCTION__);
      if ( !isset( $z2b ) ) {
         $z2b = array();
         foreach( $this->config['mappings'] as $mapping) {
            $z2b[$mapping['source']] = $mapping['target'];
         }
      }
      return $z2b;
   }
}
