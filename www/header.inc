<?php
require_once('page_data.inc');
if( !isset($pageData) && isset($id) && isset($testPath) ) {
    $options = null;
    if (array_key_exists('end', $_REQUEST))
        $options = array('end' => $_REQUEST['end']);
    $pageData = loadAllPageData($testPath, $options);
}
if (isset($pageData) && is_array($pageData)) {
  $adultKeywords = array();
  if (is_file('./settings/adult.txt'))
    $adultKeywords = file('./settings/adult.txt', FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
  foreach($pageData as &$run_data) {
      foreach($run_data as &$test_data) {
          $isAdult = false;
          if (array_key_exists('URL', $test_data))
            foreach ($adultKeywords as $keyword)
              if ((stripos($test_data['URL'], $keyword) !== false) ||
                  (array_key_exists('title', $test_data) &&
                  stripos($test_data['title'], $keyword) !== false)) {
                $isAdult = true;
                break;
              }
          if ($isAdult || (array_key_exists('adult_site', $test_data) && $test_data['adult_site'])) {
              define('ADULT_SITE', true);
              $adult_site = true;
              break 2;
          }
      }
  }
}

if( $supportsAuth && !defined('EMBED') )
{
    echo '<div id="wptAuthBar">';
    if( isset($user) )
    {
        $logoutUrl = '/forums/member.php?action=logout';
        if( isset($mybb) && strlen($mybb->user['logoutkey']) )
            $logoutUrl .= '&logoutkey=' . $mybb->user['logoutkey'];
        echo "Welcome, $user &nbsp;|&nbsp; <a href=\"$logoutUrl\">Logout</a>";
    }
    else if( isset($_COOKIE['google_email']) && isset($_COOKIE['google_id']) )
	{
        $logoutUrl = 'javascript:ClearOAuthCookies();';
        $google_email = $_COOKIE['google_email'];
        echo "Welcome, $google_email &nbsp;|&nbsp; <a href=\"$logoutUrl\">Logout</a>";
    }
    else
    {
        echo '<a href="/forums/member.php?action=login">Login</a> &nbsp;|&nbsp; ';
        echo '<a href="/forums/member.php?action=register">Register</a> &nbsp;|&nbsp; ';
        $currentPath = $_SERVER['SCRIPT_NAME'];
        echo '<a href="/oauth/login.php">Login with Google</a> ';
    }
    echo '</div>';
}

if (!defined('EMBED')) {
?>

<div id="header">
    <h1 class="logo"><a href="/">WebPageTest</a></h1>
    <div class="headerAd">
    <?php
        if( !defined('BARE_UI') )
        {
            // see if we have a location that the test was run from (for a custom ad)
            $testLoc = null;
            if( isset($adLoc) )
                $testLoc = $adLoc;
            elseif( isset($test) && isset($test['test']) && isset($test['test']['loc']) )
                $testLoc = $test['test']['loc'];
            
            if( strpos($testLoc, '..') === false && strpos($testLoc, '/') === false && strpos($testLoc, '\\') === false && 
                isset($testLoc) && strlen($testLoc) && is_file("./custom/$testLoc/headerAd.inc") )
                include("./custom/$testLoc/headerAd.inc");
            elseif( is_file('./ads/header.inc') && (!array_key_exists('HTTP_SSL', $_SERVER) || $_SERVER['HTTP_SSL'] != 'On') )
                include('./ads/header.inc');
        }
    ?>
    </div>
</div>

<div id="nav_bkg"><ul id="nav" class="cufon-dincond_black"><?php
        if ($id) {
            $resultUrl = "/results.php?test=$id";
            if (array_key_exists('end', $_REQUEST))
                $resultUrl .= "&end={$_REQUEST['end']}";
            elseif (FRIENDLY_URLS)
                $resultUrl = "/result/$id/";
        }
        if (!isset($navTabs) || !count($navTabs)) {
            $navTabs = array(  'Home' => '/' );
            if (isset($headerType) && $headerType == 'video') {
                if (!$filmstrip) {
                    $filmstrip = urldecode($_COOKIE['fs']);
                }
                if ($filmstrip) {
                    $navTabs['Test Result'] = '/video/compare.php?tests=' . urlencode($filmstrip);
                }
            } else {
                $navTabs['Test Result'] = isset($resultUrl) ? $resultUrl : '';
            }

            $navTabs['Test History'] = FRIENDLY_URLS ? '/testlog/1/' : '/testlog.php?days=1';

            if (is_dir('wptmonitor')) {
                $navTabs['Monitor'] = '/wptmonitor/';
            }
            if (is_dir('forums') && !$settings['disableMybb']) {
                $navTabs['Forums'] = '/forums/';
            }
            $navTabs['Documentation'] = 'http://sites.google.com/a/webpagetest.org/docs/';
            //if (!$privateInstall) {
            //    $navTabs['Try Acceleration'] = 'http://www.webpagetest.org/compare';
            //}
            $navTabs['About'] = FRIENDLY_URLS ? '/about' : '/about.php';
        }
        foreach ($navTabs as $tabName => $tabUrl) {
            // make sure we have a test result to navigate to
            if (strlen($tabUrl)) {
                // highlight the current tab
                $target = '';
                $current = '';
                $opens = '';
                if( !strcasecmp($tabName, $tab) )
                    $current = ' class="current"';
                if (substr($tabUrl, 0, 4) == 'http') {
                    $target = ' target="_blank"';
                    $opens = ' (opens in a new tab)';
                }
                echo "<li$current><a title=\"$tabName$opens\" href=\"$tabUrl\"$target>$tabName</a></li>";
            }
        }

        if (!$privateInstall && $_SERVER['HTTP_SSL'] != 'On' && (!defined('NAV_NO_SHARE') || !NAV_NO_SHARE))
        {
            echo '<div class="gplusone"><g:plusone size="medium" count="false" href="http://www.webpagetest.org/" actions="+1,addtocircles"></g:plusone>&nbsp;
                <a href="http://twitter.com/share" class="twitter-share-button" data-count="none" data-text=" "></a>
                ';
            if (!array_key_exists('slow', $_REQUEST)) {
                echo '<script type="text/javascript">
                        (function() {
                            var po = document.createElement(\'script\'); po.type = \'text/javascript\'; po.async = true;
                            po.src = \'https://apis.google.com/js/plusone.js\';
                            var tw = document.createElement(\'script\'); tw.type = \'text/javascript\'; tw.async = true;
                            tw.src = \'http://platform.twitter.com/widgets.js\';
                            var s = document.getElementsByTagName(\'script\')[0]; 
                            s.parentNode.insertBefore(po, s);
                            s.parentNode.insertBefore(tw, s);
                          })();
                  </script></div>
                  ';
            } else {
                echo '<script type="text/javascript">
                        (function() {
                            var po = document.createElement(\'script\'); po.type = \'text/javascript\'; po.async = true;
                            po.src = \'https://apis.google.com/js/plusone.js\';
                            var s = document.getElementsByTagName(\'script\')[0]; 
                            s.parentNode.insertBefore(po, s);
                          })();
                  </script></div>
                  ';
                if ($_REQUEST['slow'] !== 'bottom') {
                    $delay = 10;
                    if (array_key_exists('delay', $_REQUEST)) {
                        $delay = $_REQUEST['delay'];
                    }
                    echo "<script type=\"text/javascript\" src=\"/slowtweet.php?delay=$delay\"></script>";
                }
            }
        }
?></ul>

</div>

<?php
if (!defined('BARE_UI') && isset($settings['alert']) && strlen($settings['alert'])) {
    echo '<div class="alert bold">';
    echo $settings['alert'];
    echo '</div>';
}
} // EMBED
?>
    
<div id="main">
    <div class="level_2">
        <div class="content-wrap">
            <div class="content">

            
<?php
//If we're looking at a test result, include the extra header section and sub-menu
if( !strcasecmp('Test Result', $tab) && !@$nosubheader && !defined('EMBED') )
{
    // make sure the test is actually complete
    if( (isset($test['test']) && isset($test['test']['completeTime'])) )
    {
        if( !isset($pageData) )
            $pageData = loadAllPageData($testPath);
            
        $gradeRun = 1;
        if( array_key_exists('run', $_GET) && $_GET["run"] )
            $gradeRun = $_GET["run"];
        else
        {
            $medianRun = GetMedianRun($pageData, 0, $median_metric);
            if( $medianRun )
                $gradeRun = $medianRun;
        }

        echo '<div id="header_container">';
        echo '<div id="optimization">';
            echo '<div id="optimization_header">';
            if( gz_is_file("$testPath/{$gradeRun}_pagespeed.txt") )
            {
                $score = GetPageSpeedScore("$testPath/{$gradeRun}_pagespeed.txt");
                if( strlen($score) )
                {
                    $pageSpeedUrl = FRIENDLY_URLS ? "/result/$id/$gradeRun/pagespeed/" : "pagespeed.php?test=$id&run=$gradeRun";
                    echo "<span id=\"headerPagespeedScore\"><a href=\"$pageSpeedUrl\">PageSpeed {$pageData[$gradeRun][0]['pageSpeedVersion']} Score</a>: <b>$score/100</b></span>";
                }
            }
            
            echo "<div id=\"opthelp\"><a href=\"https://sites.google.com/a/webpagetest.org/docs/other-resources/optimization-resources\" title=\"Optimization Resources\" target=\"_blank\">Need help improving?</a></div>\n";
            echo '</div>';
            echo '<div id="grades">';
                $grades = GetGrades($pageData[$gradeRun][0], $test, $id, $gradeRun);
                if( count($grades) )
                {
                    $smaller = '';
                    if( count($grades) > 6 )
                      $smaller = ' smaller';
                    echo "<ul class=\"grades$smaller\">";
                    $optlink = FRIENDLY_URLS ? "/result/$id/$gradeRun/performance_optimization/" : "performance_optimization.php?test=$id&run=$gradeRun";
                    foreach( $grades as $check => &$grade )
                        echo "<li class=\"$check\"><a href=\"$optlink#$check\"><h2 class=\"{$grade['class']}\">{$grade['grade']}</h2></a>{$grade['description']}</li>";
                    echo '</ul>';
                }
            echo '</div>';
        echo '</div>';
        
        echo '<div id="header_data">';
            $shortUrl = str_replace('http://', '',  FitText($url, 120));
            echo "<h2 class=\"alternate cufon-dincond_regular\">Web Page Performance Test for<br>";
            if ($settings['nolinks']) {
                echo "<span class=\"medium colored\">$shortUrl</span>";
            } else {
                echo "<a class=\"url cufon-dincond_black\"  rel=\"nofollow\" title=\"$url\" href=\"$url\">$shortUrl</a>";
            }
            echo "</h2>";
            
            echo "<p class=\"heading_details\"><strong>From:</strong> {$test['test']['location']}<br>";
            if (isset($test['testinfo']) && (isset($test['testinfo']['completed']) || isset($test['testinfo']['started'])) )
            {
                if (isset($test['testinfo']['completed'])) {
                    $timeStr = $test['testinfo']['completed'];
                } else {
                    $timeStr = $test['testinfo']['started'];
                }
                $completed = (int)$timeStr + ($tz_offset * 60);
                $time = strftime('%x %X', $completed);
                echo "<span class=\"jsdate\" date=\"$timeStr\">$time</span><br>\n";
            }

            if( $dom ) 
                echo 'DOM Element: <b>' . $dom . '</b><br>';
            if( array_key_exists('authenticated', $test['test']) && (int)$test['test']['authenticated'] == 1)
                echo '<b>Authenticated: ' . $login . '</b><br>';
            if( (int)$test['test']['connections'] !== 0)
                 echo '<b>' . $test[test][connections] . ' Browser connections</b><br>';
            if( array_key_exists('script', $test['test']) && strlen($test['test']['script']) ) 
                echo '<b>Scripted test</b><br>';
            if( strlen($blockString) )
                echo "Blocked: <b>$blockString</b><br>";
        echo '</div>';
        echo '</div>';
        
        echo '<div id="test_results-container">';

        echo '<div id="test-1" class="test_results">';
        echo '<ul class="test_menu">';
        
        if( !$run )
            $run = $gradeRun;
        
        $cTxt = '';
        if( $cached )
            $cTxt = 'cached/';

        $resultUrl = "/results.php?test=$id";
        $details = "/details.php?test=$id&run=$gradeRun&cached=$cached";
        $domains = "/domains.php?test=$id&run=$gradeRun&cached=$cached";
        $end = '';
        if (array_key_exists('end', $_REQUEST)) {
            $resultUrl .= "&end={$_REQUEST['end']}";
            $details .= "&end={$_REQUEST['end']}";
            $end = "&end={$_REQUEST['end']}";
        } elseif (FRIENDLY_URLS) {
            $resultUrl = "/result/$id/";
            $details = "/result/$id/$gradeRun/details/$cTxt";
        }
        $tabs = array( 'Summary' => $resultUrl, 'Details' => $details);
                    
        if( $pageData[$gradeRun][$cached]['optimization_checked'] )
            $tabs['Performance Review'] = "/performance_optimization.php?test=$id&run=$gradeRun&cached=$cached$end";
        
        $cTxt2 = '';
        if( $cached )
            $cTxt2 = '_Cached';
        if( gz_is_file("$testPath/$gradeRun{$cTxt2}_pagespeed.txt") )
            $tabs['PageSpeed'] = "/pagespeed.php?test=$id&run=$gradeRun&cached=$cached$end";
        $tabs['Content Breakdown'] = "/breakdown.php?test=$id&run=$gradeRun&cached=$cached$end";
        $tabs['Domains'] = "/domains.php?test=$id&run=$gradeRun&cached=$cached$end";
        if (gz_is_file("$testPath/$gradeRun{$cTxt2}_devtools.json"))
            $tabs['Processing Breakdown'] = "/breakdownTimeline.php?test=$id&run=$gradeRun&cached=$cached$end";
        
        if( !isset($test['testinfo']) || !$test['testinfo']['noimages'] )
            $tabs['Screen Shot'] = "/screen_shot.php?test=$id&run=$gradeRun&cached=$cached$end";
        //$tabs['More Checks'] = "/moreChecks.php?test=$id";

        foreach( $tabs as $tabName => $tabUrl )
        {
            // make sure we have a test result to navigate to
            if( strlen($tabUrl) )
            {
                // highlight the current tab
                $current = '';
                if( !strcasecmp($tabName, $subtab) )
                    $current = ' class="current"';
                echo "<li$current><a href=\"$tabUrl\">$tabName</a></li>";
            }
        }
    
        echo '</ul>';
        echo '<div class="test_results-content">';
    }
    else
    {
    ?>
        <div id="test_results_running-container">
            <div id="test-1" class="test_results">
                <div class="test_results-content">
    <?php
    }
}

// include the comparison UI pop-up dialog
/*
if( !strcasecmp('Test Result', $tab) || (array_key_exists('compare', $_COOKIE) && strlen($_COOKIE['compare']))) {
    echo '<div id="compare-dlg">';
    echo 'Compare';
    echo '</div>';
}
*/

/**
* Calculate the grades for the given test
* 
*/
function GetGrades(&$pageData, &$test, $id, $run)
{
    global $test;
    $grades = array();
    if($pageData['optimization_checked'])
    {
        require_once('optimization_detail.inc.php');

        $opt = getOptimizationGrades($pageData, $test, $id, $run);

        if( $test['testinfo']['view'] != 'simple' && $test['testinfo']['view'] != 'pss' )
            $grades['first_byte_time'] = array( 'class' => $opt['ttfb']['class'], 'grade' => $opt['ttfb']['grade'], 'description' => $opt['ttfb']['label']);
        $grades['keep_alive_enabled'] = array( 'class' => $opt['keep-alive']['class'], 'grade' => $opt['keep-alive']['grade'], 'description' => $opt['keep-alive']['label']);
        $grades['compress_text'] = array( 'class' => $opt['gzip']['class'], 'grade' => $opt['gzip']['grade'], 'description' => $opt['gzip']['label']);
        $grades['compress_images'] = array( 'class' => $opt['image_compression']['class'], 'grade' => $opt['image_compression']['grade'], 'description' => $opt['image_compression']['label']);
        if (array_key_exists('progressive_jpeg', $opt))
          $grades['progressive_jpeg'] = array( 'class' => $opt['progressive_jpeg']['class'], 'grade' => $opt['progressive_jpeg']['grade'], 'description' => $opt['progressive_jpeg']['label']);
        $grades['cache_static_content'] = array( 'class' => $opt['caching']['class'], 'grade' => $opt['caching']['grade'], 'description' => $opt['caching']['label']);
        $grades['use_of_cdn'] = array( 'class' => $opt['cdn']['class'], 'grade' => $opt['cdn']['grade'], 'description' => $opt['cdn']['label']);
    }
    
    return $grades;
}
?>
<script type="text/javascript">
  markUserTime('aft.Site Header');
</script>
