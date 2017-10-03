# Roadmap

Plotting out what kinds of features to implement and in what order.

I think a good start would be getting it to function in all the ways I currently use Anki so that I might start using it myself. The most important part of becoming a project maintainer is ensuring you yourself use it on a daily basis. One simple way of doing that would be to make it inter-operable with Anki. That is, it would have its own custom model and syncing, but also keep itself synced to AnkiWeb. It would be less disruptive to my life, at least, but ultimately delay the final goal.

That's going to be a lot of work, but let's iron out the basics.

My activity is typically limited to the following.
1. Do reps (mostly mobile)
1. Add notes/media (mobile + desktop)
1. Edit notes (mobile + desktop)
1. Reschedule cards (mostly mobile)
1. Edit templates (mostly destop)
1. Sync (mobile + desktop)

For simplicity's sake, it'd probably be best to start with making an AnkiWeb-like service that I self-host so I don't need to worry about making fat clients. Granted, the architecture is exactly the same, but at least it simplifies things for now. I'd probably use [Laravel](https://laravel.com/docs/5.5/authentication) because of how easy it would be to get secure user logins all squared away. That could simply wrap a hosted [Cordova](https://cordova.apache.org/#getstarted) application and/or wrap the REST API using [Laravel's Stateless Authentication](https://laravel.com/docs/5.5/authentication#stateless-http-basic-authentication). That way I could use it on any platform out of the box so long as I have internet access.

## Platform Support:

### Desktop: Windows, Linux
- Anki Importer/Exporter - Big part will be scheduling and notes
- Basic Scheduling
- Git-backed syncing

### Mobile: Android

