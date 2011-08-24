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
   * Override parent::configForm().
   */
   public function configForm(&$form_state) {
      $types = node_get_types('names');
      $form = array();
      $form['content_type'] = array(
         '#type' => 'select',
         '#title' => t('Content type'),
         '#description' => t('Select the content type for the nodes to be created. <strong>Note:</strong> Users with "import !feed_id feeds" permissions will be able to <strong>import</strong> nodes of the content type selected here regardless of the node level permissions. Further, users with "clear !feed_id permissions" will be able to <strong>delete</strong> imported nodes regardless of their node level permissions.', array('!feed_id' => $this->id)),
         '#options' => $types,
         '#default_value' => $this->config['content_type'],
         );
      $format_options = array(FILTER_FORMAT_DEFAULT => t('Default format'));
      $formats = filter_formats();
      foreach ($formats as $format) {
         $format_options[$format->format] = $format->name;
      }
      $form['input_format'] = array(
         '#type' => 'select',
         '#title' => t('Input format'),
         '#description' => t('Select the input format for the body field of the nodes to be created.'),
         '#options' => $format_options,
         '#default_value' => $this->config['input_format'],
         );
      $author = user_load(array('uid' => $this->config['author']));
      $form['author'] = array(
         '#type' => 'textfield',
         '#title' => t('Author'),
         '#description' => t('Select the author of the nodes to be created - leave empty to assign "anonymous".'),
         '#autocomplete_path' => 'user/autocomplete',
         '#default_value' => empty($author->name) ?  'anonymous' : check_plain($author->name),
         );
      $form['authorize'] = array(
         '#type' => 'checkbox',
         '#title' => t('Authorize'),
         '#description' => t('Check that the author has permission to create the node.'),
         '#default_value' => $this->config['authorize'],
         );
      $period = drupal_map_assoc(array(FEEDS_EXPIRE_NEVER, 3600, 10800, 21600, 43200, 86400, 259200, 604800, 604800 * 4, 604800 * 12, 604800 * 24, 31536000), 'feeds_format_expire');
      $form['expire'] = array(
         '#type' => 'select',
         '#title' => t('Expire nodes'),
         '#options' => $period,
         '#description' => t('Select after how much time nodes should be deleted. The node\'s published date will be used for determining the node\'s age, see Mapping settings.'),
         '#default_value' => $this->config['expire'],
         );
      $form['update_existing'] = array(
         '#type' => 'radios',
         '#title' => t('Update existing nodes'),
         '#description' => t('Select how existing nodes should be updated. Existing nodes will be determined using mappings that are a "unique target".'),
         '#options' => array(
            FEEDS_SKIP_EXISTING => 'Do not update existing nodes',
            FEEDS_REPLACE_EXISTING => 'Replace existing nodes',
            FEEDS_UPDATE_EXISTING => 'Update existing nodes (slower than replacing them)',
            ),
         '#default_value' => $this->config['update_existing'],
         );
      return $form;
   }
   
   /**
   * Override parent::configFormValidate().
   */
   public function configFormValidate(&$values) {
      if ($author = user_load(array('name' => $values['author']))) {
         $values['author'] = $author->uid;
      }
      else {
         $values['author'] = 0;
      }
   }
   
   /**
   * Reschedule if expiry time changes.
   */
   public function configFormSubmit(&$values) {
      if ($this->config['expire'] != $values['expire']) {
         feeds_reschedule($this->id);
      }
      parent::configFormSubmit($values);
   }
   
   
   /**
   * Return available mapping targets.
   */
   public function getMappingTargets() {
      $targets = array(
         'title' => array(
            'name' => t('Title'),
            'description' => t('The title of the node.'),
            ),
         );
      // Include body field only if available.
      $type = node_get_types('type',  $this->config['content_type']);
      if ($type->has_body) {
         // Using 'teaser' instead of 'body' forces entire content above the break.
         $targets['body'] = array(
            'name' => t('Body'),
            'description' => t('The body of the node. The teaser will be the same as the entire body.'),
            );
      }
      $targets += array(
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
            ),
         );
      
      // Let other modules expose mapping targets.
      self::loadMappers();
      drupal_alter('feeds_node_processor_targets', $targets, $this->config['content_type']);
      
      return $targets;
   }
   
   public function process(FeedsImportBatch $batch, FeedsSource $source) {
            // Add biblio->zotero mappings to config; 
      // the config that we set up in the default constructor is gone
      module_load_include('inc', 'biblio_zotero', 'ZoteroBiblioMap');
      $biblioToZoteroFields = ZoteroBiblioMap::getBiblioToZoteroFields();
      foreach ( $biblioToZoteroFields as $bibkey => $zotkey ) {
         $this->config['mappings'][] = array(
            'source' => $zotkey,
            'target' => $bibkey,
            'unique' => FALSE,
            );
      }
      parent::process($batch, $source);
   }

   
   protected function map(FeedsImportBatch $batch, $target_item = NULL) {
      //dd($batch, "batch");
      
      // map the basics
      parent::map($batch, $target_item);
      
      // map the zotero fields
      $parser = feeds_importer($this->id)->parser; // $batch's fields are protected so we have to get them from the parser
      $itemType = $parser->getSourceElement($batch, 'itemType');
      $tags = $parser->getSourceElement($batch, 'tags');
      $tags = is_array($tags) ? $tags : array();
      $creators = $parser->getSourceElement($batch, 'creators');
      $creators = is_array($creators) ? $creators : array();
      $institution = $parser->getSourceElement($batch, 'institution');
      $target_item->biblio_type = ZoteroBiblioMap::zoteroToBiblioType($itemType);
      
      // map the tags
      $tagStrings = array();
      foreach ($tags as $t) {
         $tagStrings[] = $t->tag; 
      }
      
      // TODO un-hardcode
      $UNHARDCODE_THIS = 2;
      $target_item->taxonomy['tags'][$UNHARDCODE_THIS] = implode(",", $tagStrings);
      
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
		if ( module_exists('og') ) {
		   $feed_nid = $batch->feed_nid;
		   $feed = node_load($feed_nid);
		   $target_item->og_groups = $feed->og_groups;
		   $target_item->og_groups_both = $feed->og_groups_both;
		}
		
   }
   
     /**
   * Override setTargetElement to operate on a target item that is a node.
   */
  public function setTargetElement(&$target_node, $target_element, $value) {
     parent::setTargetElement($target_node, $target_element, $value);
     $biblioToZoteroFields = ZoteroBiblioMap::getBiblioToZoteroFields();
     if (in_array($target_element, array_keys($biblioToZoteroFields))) {
      $target_node->$target_element = $value;
    }
  }
}
