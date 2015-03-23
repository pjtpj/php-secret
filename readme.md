php-secret
=========

PHP Secret is a PHP extension that allows storage and retreival of a "secret". The 
intended usages scenario is in a virtual hosting environment. Because of the way
web server permissions work, it is fairly easy for one virtual host account to 
see the source files for another virtual host account. With PHP Secret, passwords
don't have to be saved in the PHP source code. Instead, they can be retrieved
via the exs_get_secret() function. It is theoretically possible for PHP Secret
to be very secure, but most hosting accounts have folder and/or files that
are wrietable by the web server user and these folders or files can be used
to defeat the security provided by PHP Secret.

Build Instructions
==================

To make a PECL package:

    cd ~/git/php-secret
    ./build.sh

To make a debian/ubuntu package:

    cd ~/ubuntu
    export DEBEMAIL="user@example.com"
    export DEBFULLNAME="User Example"
    dh-make-pecl --phpversion 5 --prefix php5- secret-1.2.0.tgz 
    cd php5-secret-1.2.0
    debuild -b -uc -us
    debuild -S -uc -us

Installation and Configuration
==============================

    dpkg -i php5-secret_1.2.0-1_amd64.deb
    dpkg -r php5-secret (to remove!)

To configure, create the file /etc/php_secret.conf, owned by root, mode 400 and 
enter the following configuration variables:

    Debug FALSE
    Host mysqldbserver
    Database secret_database
    Username secret_username
    Password mypassword
    HomeRoot /var/www/

Finally, load PHP Secret into PHP with:

    php5enmod secret
