QtWebkit Resource Interception
===========

Sometimes you need to intercept resources downloaded via QtWebKit, but since QNetworkReply (the only possible way to handle replies from network requests) is not sequental, on read attempt it's already read by webkit. You should use another aproach here: reimplemented QNetworkAccessManager and QNetworkReply proxy.

This repository provides both and example application.
