




## ----------------------------------------------------------------------------
## Vivado TCL script to help build ROE Framer Example Systems. 
## ----------------------------------------------------------------------------
namespace eval ::roe::data {
  
  ## --------------------------------------------------------------------------
  ## Return a Dict that contains all the settings we need, IP, Parts and 
  ## automation types.
  ## --------------------------------------------------------------------------
  package require yaml

  proc readYamlFileRetDict { fileNameIn } {
    set fp [open $fileNameIn r]
    set file_data [read $fp]
    close $fp
    return [yaml::yaml2dict $file_data]
  }

  proc return_config_dict { } {
    set settingsFile "my_vivado_configs.yml"
    puts "Loading settings from $settingsFile"
    set cfg [readYamlFileRetDict $settingsFile]
    return [dict get $cfg cfg]
  }

  ## Create a variable in this namespace containing all the settings.
  set script_config [return_config_dict]
  ## Add the path to this script
  dict set script_config cfg scriptDir [file dirname [file normalize [info script]]]

}

## ----------------------------------------------------------------------------
## This proc creates the actual design content on the canvas. You can override 
## this with a custom procedure. Do this by creating your own version of this  
## procedure using the code below.
## 
## namespace eval ::roe::data {
##   proc design_build { ipRepo ipName ipCfgKey } {
##     ## Your design here
##   };# End of proc
## };# End of namespace
##
## ----------------------------------------------------------------------------
namespace eval ::roe::data {
  proc design_build { ipRepo ipName ipCfgKey } {
  
    ## add the framer and configure with settings passed in as arguments
    create_bd_cell -type ip -vlnv [dict get ${::roe::data::script_config} flow ipType] ${ipName}
    set_property -dict [::roe::bin::returnIpSettings $ipCfgKey] [get_bd_cells ${ipName}]
    
    if { $ipRepo != "" } {
      puts "Manual source of BA scripts from IP repo. Factory use case."
      source ${ipRepo}/automation/auto_utils.tcl  
      ::xilinx.com_bd_rule_roe_framer::build_roe_demo_subsystem ${ipName} [::roe::bin::get_baSettings cust_repo_ptp]
    } else {
      puts "Call standard block automation"
      apply_bd_automation -rule xilinx.com:bd_rule:roe_framer -config [::roe::bin::get_baSettings norm_repo_ptp] [get_bd_cells ${ipName}]
    }
    
    ## Add QPLL fix
    add_qpll_reset ${ipName}

  }

  proc add_qpll_reset { ipName } {
     
    create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice /datapath/framer_datapath/xlslice_0
    set_property -dict [list CONFIG.DIN_TO {1} CONFIG.DIN_FROM {1} CONFIG.DIN_WIDTH {8} CONFIG.DOUT_WIDTH {1}] [get_bd_cells datapath/framer_datapath/xlslice_0]
    
    connect_bd_net [get_bd_pins datapath/framer_datapath/xlslice_0/Din]  [get_bd_pins datapath/framer_datapath/${ipName}/user_rw_out]
    connect_bd_net [get_bd_pins datapath/framer_datapath/xlslice_0/Dout] [get_bd_pins datapath/xxv_eth_subs/xxv_wrap/xxv_ethernet_0/qpllreset_in_0]

  }


}

## Privide another hook to allow constraints, custom user modifications etc
## after the main design has been built.
namespace eval ::roe::data {
  proc design_modification { projectName board  mode } {
  
    if { $board == "zcu111" } {
      add_files -copy_to ${projectName}/xdc -fileset constrs_1 -force -norecurse constraints/roe_framer_zcu111_pinout.xdc

      ## If the board is zcu111, ensure the REFCLK is 156.25MHz. The default BA
      ## can set this to 161 in 25G Mode, but we want to force this to the default
      ## rate of the Si570 on the board.
      set_property -dict [list CONFIG.GT_REF_CLK_FREQ {156.25}] [get_bd_cells datapath/xxv_eth_subs/xxv_wrap/xxv_ethernet_0]
      set_property CONFIG.FREQ_HZ 156250000 [get_bd_intf_ports /gt_ref_clk]

      validate_bd_design
      set_property synth_checkpoint_mode None [get_files *.bd]
      generate_target all [get_files *.bd]

    }

    if { [get_property CONFIG.LINE_RATE [get_bd_cells /datapath/xxv_eth_subs/xxv_wrap/xxv_ethernet_0]] == 25 } {
      ## Adjust the timer sync clock period
      set_property -dict [list CONFIG.RESYNC_CLK_PERIOD {2560}] [get_bd_cells datapath/xxv_eth_subs/xxv_wrap/support_1588_2step/timer1588_subs/timer_sync_rx]
      set_property -dict [list CONFIG.RESYNC_CLK_PERIOD {2560}] [get_bd_cells datapath/xxv_eth_subs/xxv_wrap/support_1588_2step/timer1588_subs/timer_sync_tx]
    }
    
    if {[regexp {om5} $mode] == 1} {
      ## Add additional xdc constraint for ORAN mode.
      add_files -copy_to ${projectName}/xdc -fileset constrs_1 -force -norecurse constraints/roe_framer_xdc_fifosync.xdc
    }

  }
}

## ----------------------------------------------------------------------------
## 
## ----------------------------------------------------------------------------
namespace eval ::roe::bin {

  ## --------------------------------------------------------------------------
  ## Script Usage
  ## --------------------------------------------------------------------------
  proc usage { } {
  
    set mode   om5
    set ipRepo "[pwd]/path_to_repo"
    set board  zcu111
    
    puts "
##-----------------------------------------------------------------------------
## Xilinx roe_framer Example System Starter Script.
## Use this to generate XSA for Petalinux.
## Extend to build your own System flows.
##-----------------------------------------------------------------------------
## Manually set variables
set mode   om5
set board  zcu111
set ipRepo \"\"
set doimpl     0
set exitOnDone 0

## Review the information in the configuration dict
::roe::bin::reload

## Review the information in the configuration dict
::roe::bin::showData

## Build command
::roe::bin::createIp_runIpiBuild <Optional IP Repo> <Build Mode(om0|om5)> <Target Board(zcu102|zcu111)>

## Default
::roe::bin::createIp_runIpiBuild $ipRepo $mode $board

## Once complete you can call implementation & then generate the XSA
::roe::bin::run_impl
::roe::bin::exportHw

##-----------------------------------------------------------------------------"
  }

  ## --------------------------------------------------------------------------
  ## Create a variable in this namespace and assign the name of this script to it
  set fileName [file normalize [info script]]
  
  ## reload this script
  proc reload { } {
    source $::roe::bin::fileName
  }

  ## Dump all the script config data
  proc showData { } {
    foreach var [dict keys ${::roe::data::script_config} ] {
      puts "[format %-15s $var] : [dict get ${::roe::data::script_config} $var]"
    }
  }
  
  ## --------------------------------------------------------------------------
  ## Procs to pull nested values of from the configuration DICT with error 
  ## messaging should the values not exist.
  proc getL1DictSettings { {caller ""} {l1_key ""} {select ""} } {

    if { [dict exists ${::roe::data::script_config} $l1_key] } {
      if { [dict exists ${::roe::data::script_config} $l1_key $select] } {
        return [dict get ${::roe::data::script_config} $l1_key $select]
      } else {
        puts "Error $select does not exist for $caller"
        return ""
      }

    } else {
      puts "Error dict \[::roe::data::script_config $l1_key\] does not exist"
      return ""
    }

  }  

  proc getL2DictSettings { {caller ""} {l1_key ""} {l2_key ""} {select ""} } {

    if { [dict exists ${::roe::data::script_config} $l1_key] } {
      if { ! [dict exists ${::roe::data::script_config} $l1_key $l2_key] } {
        puts "Error L2 Key $l2_key has no settings"
        return "error $caller L2 Key"
      }
      if { ! [dict exists ${::roe::data::script_config} $l1_key $l2_key $select] } {
        puts "Error L2 Key $l2_key has no settings for Key $select"
        return "error $caller L2 Key Select"
      }
      return [dict get ${::roe::data::script_config} $l1_key $l2_key $select]
    } else {
      puts "Error dict \[::roe::data::script_config $l1_key\] does not exist"
    }
    return ""

  }  

  ## --------------------------------------------------------------------------
  ## Procs to pull values from the configuration dict. Fetch has a meaningful
  ## name and tests for existance each time.
  ## --------------------------------------------------------------------------
  proc returnIpSettings { {select ""} } {
    return [getL1DictSettings returnIpSettings ip $select]
  }  

  proc get_board { board name } {
    return [getL2DictSettings get_board boards $board $name]
  }
  
  proc get_baSettings { name } {
    return [getL1DictSettings get_baSettings bacfg $name]  
  }
  
  ## --------------------------------------------------------------------------
  ## Functions to build the design. This holds minimal Vivado TCL mechanics
  ## to get to a validated design and only bitstream and XSA.
  ## --------------------------------------------------------------------------
  proc create_projName { board mode noDateInProjName} {
    ## Create a uniqueish name based in the board, project name and Vivado
    ## version in use. The chosen name is last as this is likely to change/incrment
    ## during development runs. Therefore directory names should sort nicely
    ## on the users file browser.
    if { $noDateInProjName } {
      set pName "${board}_${mode}_[version -short]"
    } else {
      set pName "${board}_${mode}_[version -short]__[clock format [clock seconds] -format "%Y%m%d_%H%M%S"]"
    }
    ## Replace chars we dont want in the project name.
    regsub {\.+} $pName "_" pName
    regsub {\s+} $pName "_" pName
    puts "Creating project ${pName} & running Block Automation"
    return $pName
  }
  
  proc run_impl { } {
    ## Launch impl
    launch_runs impl_1 -to_step write_bitstream -jobs 4
    wait_on_run impl_1
    exportHw
    return impl_1
  }
  
  proc exportHw { } {

    ## Generate XSA/HDF for Petalinux
    set  dir     [get_property DIRECTORY [current_project]]
    set  name    [get_property NAME [current_project]]
    set  sdkPath ${dir}/${name}.sdk
    file mkdir   ${sdkPath}
  
    ## Post 2019.2 you can only generate XSA, the hdf replacement. Deal with
    ## older versions
    if { [catch {write_hw_platform -fixed -force -include_bit -file ${sdkPath}/${name}.xsa} errMess] } {
  
      puts "Fall back to HDF output, placing in ${sdkPath}"
      file copy -force ${dir}/${name}.runs/impl_1/design_1_wrapper.sysdef ${sdkPath}/design_1_wrapper.hdf
  
    }

  }

  ## Select the part/board we want, create a project anme and the project
  proc configure_project { ipRepo mode board noDateInProjName } {
    ## 
    set PART  [get_board $board PART]
    set BOARD [get_board $board BOARD]
    set pName [create_projName $board $mode $noDateInProjName]
  
    create_project ${pName} ${pName} -part $PART -force
    set_property board_part $BOARD [current_project]
  
    if { $ipRepo != "" } {
      ## Add a custom IP repo
      set_property  ip_repo_paths  ${ipRepo} [current_project]
      update_ip_catalog
    }

    return $pName
  
  }
  
  ## Create project/Board Design/Example design and validate
  proc createIp_runIpiBuild { ipRepo mode board noDateInProjName } {

    set ipName roe_framer_0
    set pName [configure_project $ipRepo $mode $board $noDateInProjName]

    create_bd_design "design_1"
  
    ## Call the design script, separate proc so it can be loaded seperately.
    ::roe::data::design_build $ipRepo $ipName $mode
  
    ## Validate and write BD tcl
    validate_bd_design
    write_bd_tcl -force -no_ip_version -exclude_layout ${pName}.tcl
    
    return $pName
  
  }

  ## Pretty print the timing stats post build.
  proc reportImplStatus {runName} {
     
     ## Get some useful stats on tun
     set STATS_TNS [get_property STATS.TNS [get_runs $runName]]
     set STATS_WNS [get_property STATS.WNS [get_runs $runName]]
     set DIRECTORY [get_property DIRECTORY [get_runs $runName]]
  
     foreach stat [list_property [get_runs $runName]] { if {[regexp STATS\..+ $stat]} { 
       puts "      -> [format %-40s $stat] [get_property $stat [get_runs $runName]]" }
     }
  
     puts "\nOutput generated in            : $DIRECTORY"
  
     if { $STATS_TNS < 0 } {
        puts "Error: timing FAILED on run $runName TNS: $STATS_TNS WNS: $STATS_WNS\n"
        return 1
     } else {
        puts "Timing PASSED on run $runName TNS: $STATS_TNS WNS: $STATS_WNS\n"
        return 0
     }
  
  }

}

## ----------------------------------------------------------------------------
## Example of how you can use the -tclargs switch in Vivado to selct the build
## you want to run. Alternatively, you can just source this script and use
## function calls provided by it.
## ----------------------------------------------------------------------------
## Process the command line arguments
proc process_tclargs { argc argv } {
  if {$argc > 0} {
  
    ## Flow control variables
    set mode             om5
    set board            zcu102
    set ipRepo           ""
    set doimpl           0
    set exitOnDone       0
    set startGui         0
    set noDateInProjName 0
  
    ## This style uses a set of strings concatenated together to tell the 
    ## script what mode it should build in
    ## 1st TCLARGS = Board to target
    ## 2nd TCLARGS = IP Mode
    ## 3rd TCLARGS = Command mode to run
    ## 4rd TCLARGS = IP Directory
  
    if {$argc > 0} {
      set argIn  [string tolower [lindex $argv 0]]
      puts "Using tclargs lowercased to $argIn - Using as board used"
      set board $argIn
    }
  
    if {$argc > 1} {
      set argIn  [string tolower [lindex $argv 1]]
      puts "Using tclargs lowercased to $argIn - Using as ip config mode"
      set mode $argIn
    }

    if {$argc > 2} {
      set argIn  [string tolower [lindex $argv 2]]
      puts "Using tclargs lowercased to $argIn"
      if { [regexp {gui}    $argIn] == 1}     { set startGui   1   }
      if { [regexp {impl}   $argIn] == 1}     { set doimpl     1   }
      if { [regexp {exit}   $argIn] == 1}     { set exitOnDone 1   }
      if { [regexp {nodate} $argIn] == 1}     { set noDateInProjName 1   }
    }
  
    if {$argc > 3} {
      set argIn  [string tolower [lindex $argv 3]]
      puts "Using tclargs lowercased to $argIn - Using as IP Repo"
      set ipRepo $argIn
    }
  
    puts "**********************************************************************"
    puts "Launching\n::roe::bin::createIp_runIpiBuild $ipRepo $mode $board"
    puts "**********************************************************************"
    set projectName [::roe::bin::createIp_runIpiBuild $ipRepo $mode $board $noDateInProjName]

    puts "roePROJECTNAME: $projectName"

    ## Add board specific constraints in automation.
    ::roe::data::design_modification $projectName $board  $mode
  
    ## Run the implementation
    if { $doimpl == 1 } {
    
      puts "**********************************************************************"
      puts "Launching Implementation, this will take about an hour, with no log to screen."
      set runName [::roe::bin::run_impl]
      puts "**********************************************************************"
      ::roe::bin::reportImplStatus $runName
      puts "**********************************************************************"
    } 

    if { $startGui } {
      start_gui
    }
  
    if { $exitOnDone } {
      exit
    }
  
  }
} 

## ----------------------------------------------------------------------------
## Source a custom user file, this allows over-riding of any of the procs
## defined so far.
## ----------------------------------------------------------------------------
set USER_TCL_FILE "user_procs.tcl"
if { [file exists $USER_TCL_FILE] } {
  puts "You want to do someting cool, sourcing your stuff in $USER_TCL_FILE"  
  source $USER_TCL_FILE
}

## ----------------------------------------------------------------------------
## Process the command line arguments, show the help text
## ----------------------------------------------------------------------------
if { [info exists argc] } {
  ::roe::bin::usage
  process_tclargs $argc $argv
  ::roe::bin::usage
} else {
  ::roe::bin::usage
}
