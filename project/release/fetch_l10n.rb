#!/usr/bin/env ruby
#
# Ruby script for pulling l10n translations for digikam and kipi-plugins
# Requires ruby version >= 1.9
# 
# originally a Ruby script for generating Amarok tarball releases from KDE SVN
# (c) 2005 Mark Kretschmann <kretschmann@kde.org>
# Some parts of this code taken from cvs2dist
# License: GNU General Public License V2

require 'rbconfig'
isWindows = RbConfig::CONFIG['host_os'] =~ /mswin|mingw|cygwin/i

branch = "trunk"
tag = ""

unless $*.empty?()
    case $*[0]
        when "--branch"
            branch = `kdialog --inputbox "Enter branch name: " "branches/stable"`.chomp()
        when "--tag"
            tag = `kdialog --inputbox "Enter tag name: "`.chomp()
        else
            puts("Unknown option #{$1}. Use --branch or --tag.\n")
    end
end

# Using anonsvn so not necessary anymore
# Ask user for targeted application version
#user = `kdialog --inputbox "Your SVN user:"`.chomp()
#protocol = `kdialog --radiolist "Do you use https or svn+ssh?" https https 0 "svn+ssh" "svn+ssh" 1`.chomp()

puts "\n"
puts "**** l10n ****"
puts "\n"

i18nlangs = []
if isWindows
    i18nlangs = `type .\\project\\release\\subdirs`
else
    i18nlangs = `cat project/release/subdirs`
end

if !(File.exists?("po") && File.directory?("po"))
    Dir.mkdir( "po" )
end
Dir.chdir( "po" )
topmakefile = File.new( "CMakeLists.txt", File::CREAT | File::RDWR | File::TRUNC )
i18nlangs.each_line do |lang|
    lang.chomp!()
    if (lang != nil && lang != "")
        if !(File.exists?(lang) && File.directory?(lang))
            Dir.mkdir(lang)
        end
        Dir.chdir(lang)
        for part in ['digikam','kipiplugin_acquireimages','kipiplugin_advancedslideshow','kipiplugin_batchprocessimages','kipiplugin_calendar','kipiplugin_dngconverter','kipiplugin_expoblending','kipiplugin_facebook','kipiplugin_flashexport','kipiplugin_flickrexport','kipiplugin_galleryexport','kipiplugin_gpssync','kipiplugin_htmlexport','kipiplugin_imageviewer','kipiplugin_ipodexport','kipiplugin_jpeglossless','kipiplugin_kioexportimport','kipiplugin_metadataedit','kipiplugin_picasawebexport','kipiplugin_piwigoexport','kipiplugin_printimages','kipiplugin_rawconverter','kipiplugin_removeredeyes','kipiplugin_sendimages','kipiplugin_shwup','kipiplugin_smug','kipiplugins','kipiplugin_timeadjust' ]
    # Do not include kipiplugin_wallpaper for now as the plugin is disable.
            puts "Copying #{lang}'s #{part} over..  "
            if isWindows
                `svn cat svn://anonsvn.kde.org/home/kde/#{branch}/l10n-kde4/#{lang}/messages/extragear-graphics/#{part}.po > #{part}.po`
            else
                #`svn cat #{protocol}://#{user}@svn.kde.org/home/kde/#{branch}/l10n-kde4/#{lang}/messages/extragear-graphics/#{part}.po 2> /dev/null | tee #{part}.po `
                `svn cat svn://anonsvn.kde.org/home/kde/#{branch}/l10n-kde4/#{lang}/messages/extragear-graphics/#{part}.po 2> /dev/null | tee #{part}.po `
            end

            if FileTest.size( "#{part}.po" ) == 0
                File.delete( "#{part}.po" )
                puts "Delete File #{part}.po"
            end
    
            makefile = File.new( "CMakeLists.txt", File::CREAT | File::RDWR | File::TRUNC )
            makefile << "file(GLOB _po_files *.po)\n"
            makefile << "GETTEXT_PROCESS_PO_FILES( #{lang} ALL INSTALL_DESTINATION ${LOCALE_INSTALL_DIR} ${_po_files} )\n"
            makefile.close()
    
            puts( "done.\n" )
        end
    
         # we add the translation for kgeomap which is in extragear-libs
        for part in ['libkgeomap']
            puts "Copying #{lang}'s #{part} over..  "
            if isWindows
                `svn cat svn://anonsvn.kde.org/home/kde/#{branch}/l10n-kde4/#{lang}/messages/extragear-libs/#{part}.po > #{part}.po `
            else
                #`svn cat #{protocol}://#{user}@svn.kde.org/home/kde/#{branch}/l10n-kde4/#{lang}/messages/extragear-libs/#{part}.po 2> /dev/null | tee #{part}.po `
                `svn cat svn://anonsvn.kde.org/home/kde/#{branch}/l10n-kde4/#{lang}/messages/extragear-libs/#{part}.po 2> /dev/null | tee #{part}.po `
            end

            if FileTest.size( "#{part}.po" ) == 0
                File.delete( "#{part}.po" )
                puts "Delete File #{part}.po"
            end

            puts( "done.\n" )
        end
    
        # libkvkontakte is in kdereview but will be moved soon.
        for part in ['libkvkontakte']
            puts "Copying #{lang}'s #{part} over..  "
            if isWindows
                `svn cat svn://anonsvn.kde.org/home/kde/#{branch}/l10n-kde4/#{lang}/messages/kdereview/#{part}.po > #{part}.po `
            else
                #`svn cat #{protocol}://#{user}@svn.kde.org/home/kde/#{branch}/l10n-kde4/#{lang}/messages/kdereview/#{part}.po 2> /dev/null | tee #{part}.po `
                `svn cat svn://anonsvn.kde.org/home/kde/#{branch}/l10n-kde4/#{lang}/messages/kdereview/#{part}.po 2> /dev/null | tee #{part}.po `
            end

            if FileTest.size( "#{part}.po" ) == 0
                File.delete( "#{part}.po" )
                puts "Delete File #{part}.po"
            end
    
            puts( "done.\n" )
        end

        for part in ['libkipi']
          puts "Copying #{lang}'s #{part} over..  "
          if isWindows
            `svn cat svn://anonsvn.kde.org/home/kde/#{branch}/l10n-kde4/#{lang}/messages/kdegraphics/#{part}.po > #{part}.po `
          else
            `svn cat svn://anonsvn.kde.org/home/kde/#{branch}/l10n-kde4/#{lang}/messages/kdegraphics/#{part}.po 2> /dev/null | tee #{part}.po `
          end
          if FileTest.size( "#{part}.po" ) == 0
            File.delete( "#{part}.po" )
            puts "Delete File #{part}.po"
          end

          puts( "done.\n" )
        end

        Dir.chdir("..")
        topmakefile << "add_subdirectory( #{lang} )\n"
    end
end

puts "\n"
