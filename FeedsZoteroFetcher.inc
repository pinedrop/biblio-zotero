<?php

/**
* @file
* FeedsZoteroFetcher
*/

/**
* Fetches data via HTTP.
*/
class FeedsZoteroFetcher extends FeedsHTTPFetcher {
   
   /**
   * Expose source form.
   */
   public function sourceForm($source_config) {
      $form = array();
      $form['zotero_type'] = array(
         '#type' => 'radios',
         '#title' => t('What type of Zotero library is this?'),
         '#options' => array('group' => 'Group', 'user' => 'User'),
         '#default_value' => isset($source_config['zotero_type']) ? $source_config['zotero_type'] : '',
         '#maxlength' => NULL,
         '#required' => TRUE,
         );
      $form['zotero_id'] = array(
         '#type' => 'textfield',
         '#title' => t('Zotero ID'),
         '#description' => t('Enter the numeric id of your zotero user or group. To find your 
            group id, navigate to your groups\'s settings page and the numeric part of the URL 
            is you group id. Your userID is in
            <a href="https://www.zotero.org/settings/keys">Feeds/API section</a> of your zotero settings.'),
         '#default_value' => isset($source_config['zotero_id']) ? $source_config['zotero_id'] : '',
         '#maxlength' => NULL,
         '#required' => TRUE,
         );
      $form['api_key'] = array(
         '#type' => 'textfield',
         '#title' => t('Zotero API key'),
         '#description' => t('If your group or user account is private, you will need to enter 
            your api key to be allowed to access the library.  Your api key is in
            <a href="https://www.zotero.org/settings/keys">Feeds/API section</a> of your zotero settings.'),
         '#default_value' => isset($source_config['api_key']) ? $source_config['api_key'] : '',
         '#maxlength' => NULL,
         );
      
      $form['source'] = array(
         '#type' => 'hidden',
         '#title' => t('URL'),
         '#disabled' => TRUE,
         '#description' => t('Enter a feed URL.'),
         '#default_value' => isset($source_config['source']) ? $source_config['source'] : '',
         '#maxlength' => NULL,
         );
      $form['next_index'] = array(
         '#type' => 'hidden',
         '#title' => t('Next Index'),
         '#disabled' => TRUE,
         '#description' => t('The next index to be used as the start parameter for the next import.'),
         '#default_value' => isset($source_config['next_index']) ? $source_config['next_index'] : 0,
         '#maxlength' => NULL,
         );
      $form['child_source'] = array(
         '#type' => 'hidden',
         '#title' => t('Child source'),
         '#disabled' => TRUE,
         '#description' => t('The service url to fetch children of a given zotero item.'),
         '#default_value' => isset($source_config['child_source']) ? $source_config['child_source'] : '',
         '#maxlength' => NULL,
         );
      $form['limit'] = array(
         '#type' => 'textfield',
         '#title' => t('Limit'),
         '#description' => t('How many zotero items to fetch at each import (1-99).'),
         '#default_value' => isset($source_config['limit']) ? $source_config['limit'] : 50,
         '#maxlength' => NULL,
         );
      if ( ! empty($source_config['source'])) {
         $form['source'] = array(
            '#type' => 'markup',
            '#value' => '<p><label>URL</label>: ' . $source_config['source'] . '</p>',
            );
      }
      return $form;
   }
   
   /**
   * Override parent::sourceFormValidate().
   */
   public function sourceFormValidate(&$values) {
      // We need to construct a url that looks like this: 
      // https://api.zotero.org/groups/98765/items
      // or this:
      // https://api.zotero.org/users/12345/items
      
      if (! is_numeric($values['zotero_id'])) {
         $form_key = 'feeds][' . get_class($this) . '][zotero_id';
         form_set_error($form_key, t('The zotero id must be numeric.'));
      }

      if (! is_numeric($values['limit']) || $values['limit'] > 99 || $values['limit'] < 1 ) {
         $form_key = 'feeds][' . get_class($this) . '][limit';
         form_set_error($form_key, t('The value for limit must be a number between 1-99.'));
      }
      
      $this->setConfigSourceUrl($values);
      
      // TODO add option to fetch from zotero without json for use in non-biblio fields, i.e. the standard processor
      // TODO test the accessibility of the API call.
      
      if (!feeds_valid_url($values['source'], TRUE)) {
         $form_key = 'feeds][' . get_class($this) . '][source';
         form_set_error($form_key, t('The URL %source is invalid.', array('%source' => $values['source'])));
      }
   }
   
      /**
   * Override parent::configDefaults().
   */
   public function configDefaults() {
      $defaults = parent::configDefaults();
      return $defaults;
   }
   
   /**
   * Override parent::configForm().
   */
   public function configForm(&$form_state) {
      $form = array();
      return $form; 
   }
   
  /**
   * Implements FeedsFetcher::fetch().
   * Overrides FeedsHTTPFetcher->fetch
   */
  public function fetch(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    $result = new FeedsHTTPFetcherResult($source_config['source']);
    
    //alter the config for next fetch before saving new config
    $feed_raw_xml = $result->getRaw();
    //dd($feed_raw_xml, "feed_raw_xml");
    $feed_simple_xml = simplexml_load_string($feed_raw_xml);
    $next = $feed_simple_xml->xpath("*[@rel='next']"); 
    $first = $feed_simple_xml->xpath("*[@rel='first']"); 
    if ( ! empty( $next ) ) {
       $next_href = (string)$next[0]['href'];
       $pattern = "/start=\d+/";
       preg_match($pattern, $next_href, $matches);
       $next_index = array_pop( explode("=", $matches[0]) );
       $source_config['next_index'] = $next_index; // we have to save this too so we dont lose our page in case a user resaves the feed source form
    }
    else {
       $source_config['next_index'] = 0;
    }
    $this->setConfigSourceUrl($source_config);
    $source->setConfigFor($this, $source_config);
    $source->save();
    return $result;
  }
  
  private function setConfigSourceUrl(&$values) {
     $zotType = $values['zotero_type'] == 'group' ? 'groups' : 'users';
      $values['source'] = sprintf('https://api.zotero.org/%s/%d/items/top', $zotType, $values['zotero_id']);
      $values['source'] .= '?content=json';
      $values['source'] .= sprintf('&limit=%d',$values['limit'] );
      $values['source'] .= sprintf('&start=%d',$values['next_index'] );  // next_index is changed after every fetch
      $values['child_source'] = sprintf('https://api.zotero.org/%s/%d/items/%%s/children', $zotType, $values['zotero_id']); // the last token is replaced during parsing
      $values['child_source'] .= '?content=json&limit=99';
      if ( ! empty( $values['api_key'] ) ) {
         // TODO validate the api_key
         $values['source'] .= '&key=' . $values['api_key'];
         $values['child_source'] .= '&key=' . $values['api_key'];
      }
  }
}
