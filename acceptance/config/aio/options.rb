{
  :type => 'aio',
  :log_level => 'verbose',
  :helper => 'lib/helper.rb',
  :is_puppetserver => true,
  :puppetservice => 'puppetserver',
  :'puppetserver-confdir' => '/etc/puppetlabs/puppetserver/conf.d',
  :pre_suite => [
    'setup/common/000-delete-puppet-when-none.rb',
    'setup/aio/010_Install.rb',
    'setup/aio/021_InstallAristaModule.rb',
    'setup/common/022_Remove_LD_PRELOAD.rb',
    'setup/common/030_StopPuppetService.rb',
    'setup/common/035_StartPuppetServer.rb',
    'setup/common/040_ValidateSignCert.rb',
    'setup/common/045_SetPuppetServerOnAgents.rb',
    'setup/common/050_Setup_Broker.rb',
    'setup/common/060_Setup_PCP_Client.rb',
  ],
  :post_suite => [
    'teardown/common/099_Archive_Logs.rb',
  ],
}
