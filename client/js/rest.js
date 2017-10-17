/**
 * A basic REST interface to the Kioku serverside API.
 * This interface attempts to conform to the JSON API as specified on http://jsonapi.org/format/.
 * Perhaps against my better judgment, I've decided to roll-my-own.
 * This is in the interest of simplicity and portability.
 * I'm willing to take on some minimal jQuery as I suspect users will wish to utilize it.
 * I'm also willing to consider frameworks that detect browser support.
 * I'd like to make this reusable and easy to wrap with different strategies, even less portable ones.
 * To that end, this API doesn't do any actual sending/receiving of requests - it merely formats them.
 */

var srs = {
  endpoint : {
    port : ":8000",
    host : "localhost",
    path : undefined,
    full : undefined
  },
  // Ensures http://jsonapi.org/format/#content-negotiation-clients is conformed to.
  ValidateRequest : function(http_headers)
  {
    return true; /// \todo imlement this with error codes for each kind of violation.
  },
  // Ensures http://jsonapi.org/format/#content-negotiation-servers is conformed to.
  ValidateResponse : function(http_headers)
  {
    return true; /// \todo imlement this with error codes for each kind of violation.
  },
  // Setup the endpoint.
  Init : function()
  {
    // Configure endpoint
    self.endpoint.full = self.endpoint.host +
      (self.endpoint.port != undefined ? ":" + self.endpoint.port : "") +
      "/" +
      (self.endpoint.path != undefined ? self.endpoint.path + "/" : "");
    return self.endpoint.full;
  },
  // Tests the REST server to see if it is "sane" by seeing if it implements everything this API expects.
  Test : {
    send : function()
    {
      return {};
    },
    recv : function()
    {
      return {};
    }
  },
  GetVersion : {
    send : function()
    {
      return {};
    },
    recv : function()
    {
      return {};
    }
  },
  GetCardList : {
    send : function()
    {
      return {};
    },
    recv : function()
    {
      return {};
    }
  }
};

