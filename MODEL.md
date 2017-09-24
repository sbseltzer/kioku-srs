/user-name
  /addons - where addon scripts go
    /my-addon-folder
      -- script files go here
      some-script.[lua|rb|py]
  /templates
    /note-template-id
      generator.(lua|rb|py) - used to describe how it generates cards from a note
      sides.(txt|lua|json|xml|csv) - used to describe how many sides the note has
      fields.(txt|lua|json|xml|csv) - used to describe the fields available to the note
      /sides
        -- Display templates for card sides as named in the sides file --
        side-name-A.(html?|md|markdown|txt)
        ...
        side-name-Z.(html?|md|markdown|txt)
        -- End sides --
        /resource
          -- Extra arbitrary files used by this template
          -- When a foreign note type is imported, resources go here by default to prevent name conflicts
    /resource
      -- extra arbitrary files used by all templates
  /decks
    /deck-id
      manifest.txt
      /card-id
        .template - solely refers to a note id
        .schedule - solely contains scheduling data for the card
        fields-import.[txt|lua|json|xml|csv] - A special override file for defining all fields in one file. This is useful for import/export of foreign note types or translating between incompatible ones.
        /fields
          -- Data for fields as named in the note template fields file --
          field-data-A.(html?|md|markdown|txt)
          ...
          field-data-Z.(html?|md|markdown|txt)
          -- End fields
  /media
    -- arbitrary media files for use in decks
    filenameA
    filenameB
    ...
    filenameZ
