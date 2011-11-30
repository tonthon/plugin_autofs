Plugin Autofs-nfs for fusiondirectory
=====================================

This plugin has been tested with Openldap 2.4.23

Install and requisites
----------------------

This plugin requires the following ldap schema::

        attributetype ( 1.3.6.1.1.1.1.26 NAME 'nisMapName'
        SUP name )

        attributetype ( 1.3.6.1.1.1.1.27 NAME 'nisMapEntry'
        EQUALITY caseExactIA5Match
        SUBSTR caseExactIA5SubstringsMatch
        SYNTAX 1.3.6.1.4.1.1466.115.121.1.26{1024} SINGLE-VALUE )

        objectclass ( 1.3.6.1.1.1.2.9 NAME 'nisMap'
        DESC 'A generic abstraction of a NIS map'
        SUP top STRUCTURAL
        MUST nisMapName
        MAY description )

        objectclass ( 1.3.6.1.1.1.2.10 NAME 'nisObject'
        DESC 'An entry in a NIS map'
        SUP top STRUCTURAL
        MUST ( cn $ nisMapEntry $ nisMapName )
        MAY description )


It is available in the `rfc2307bis.schema <http://tools.ietf.org/id/draft-howard-rfc2307bis-00.txt>`_

TODO: say how to install.


How to proceed
--------------

#. Add the schema to your ldap server instance.
#. Configure an NFS server with some exports.
#. Open fusiondirectory, tell this plugin about the mount points.
#. Configure your autofs clients to look for information on the LDAP server.

Limitations
-----------

Only indirect mount could be configured.

``nisMapEntry`` being a ``IA5String`` in the LDAP schema, only plain old boring ASCII mount points and exports are supported.
