php-secret
=========

PHP Secret is a PHP extension that allows storage and retrieval of a "secret". The 
intended usage scenario is in a virtual hosting environment. Because of the way
web server permissions work, it is fairly easy for one virtual host account to 
see the source files for another virtual host account. With PHP Secret, passwords
don't have to be saved in the PHP source code. Instead, they can be retrieved
via the exs_get_secret() function. It is theoretically possible for PHP Secret
to be very secure, but most hosting accounts have folders and/or files that
are writeable by the web server user and these folders or files can be used
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
    
As you can see from the above, the user's secret is saved in a MySQL table. The current source code simplistically expects the schema of this table to support the following queries:

    SELECT WebServerSecret FROM Accounts WHERE WebServerUser = '%s'
    UPDATE Accounts SET WebServerSecret = '%s' WHERE WebServerUser = '%s'

Assuming you already have a table with user accounts, you should create a view of that table that supports the above queries, then create a mysql user that is limited to selecting and updating the WebServerSecret column of the Accounts table.

Finally, load PHP Secret into PHP with:

    php5enmod secret
    
Usage
=====
For end user, using PHP Secret is as simple as:

    exs_put_secret('This is top secret!');
    $mySecret = exs_get_secret();
    
Since any string value can be saved and retreived, you can use URL encoded name value pairs, JSON, etc..
Hosting providers may want to add some type of web accessible user interface to edit the account's secret interactive.
    
