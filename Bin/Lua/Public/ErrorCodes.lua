--[[
ДэЮѓТы
--]]

ErrorCode = {
    "FAILE",
    "OK",
    "ERROR",
}
ErrorCode = table.enum(ErrorCode, -1)

--HttpДэЮѓТы
HTTP_OK	= 200 --/**< request completed ok */
HTTP_NOCONTENT = 204 --/**< request does not have content */
HTTP_MOVEPERM =	301	--/**< the uri moved permanently */
HTTP_MOVETEMP = 302 --/**< the uri moved temporarily */
HTTP_NOTMODIFIED = 304 --/**< page was not modified from last */
HTTP_BADREQUEST	= 400 --/**< invalid http request was made */
HTTP_NOTFOUND = 404 --/**< could not find content for uri */
HTTP_BADMETHOD = 405 --/**< method not allowed for this uri */
HTTP_ENTITYTOOLARGE = 413 --/**<  */
HTTP_EXPECTATIONFAILED = 417 --/**< we can't handle this expectation */
HTTP_INTERNAL = 500 --/**< internal error */
HTTP_NOTIMPLEMENTED = 501 --/**< not implemented */
HTTP_SERVUNAVAIL = 503 --/**< the server is not available */
