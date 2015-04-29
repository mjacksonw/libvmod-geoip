/**
 * libvmod-geoip - varnish interface to MaxMind's GeoIP library
 * GeoIP API: http://www.maxmind.com/app/c
 *
 * See file README.rst for usage instructions
 * 
 * This code is licensed under a MIT-style License, see file LICENSE
*/

#include <stdio.h>
#include <stdlib.h>
#include <GeoIP.h>
#include <GeoIPCity.h>

#include "vrt.h"
#include "vrt_obj.h"
#include "cache/cache.h"

#include "vcc_if.h"


int
init_function(struct vmod_priv *pp, const struct VCL_conf *conf)
{
	// first call to lookup functions initializes pp
	return (0);
}

static void
init_priv(struct vmod_priv *pp) {
	// The README says:
	// If GEOIP_MMAP_CACHE doesn't work on a 64bit machine, try adding
	// the flag "MAP_32BIT" to the mmap call. MMAP is not avail for WIN32.
	pp->priv = GeoIP_new(GEOIP_MMAP_CACHE);
	if (pp->priv != NULL) {
		pp->free = (vmod_priv_free_f *)GeoIP_delete;
		GeoIP_set_charset((GeoIP *)pp->priv, GEOIP_CHARSET_UTF8);
	}
}

static const char *_mk_Unknown(const char *p)
{
    return p ? p : "Unknown";
}

GeoIPRecord *
vmod_geoip_record(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
{
	if (!pp->priv) {
		init_priv(pp);
	}

	return GeoIP_record_by_addr((GeoIP *)pp->priv, ip);
}

VCL_STRING
vmod_country_code(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
{
	GeoIPRecord *gir = vmod_geoip_record(ctx, pp, ip);
	return WS_Copy(ctx->ws, _mk_Unknown(gir->country_code), strlen (_mk_Unknown(gir->country_code)));
}

VCL_STRING
vmod_country_name(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
{
	GeoIPRecord *gir = vmod_geoip_record(ctx, pp, ip);
	return WS_Copy(ctx->ws, _mk_Unknown(gir->country_name), strlen (_mk_Unknown(gir->country_name)));
}

VCL_STRING
vmod_region_code(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
{
	GeoIPRecord *gir = vmod_geoip_record(ctx, pp, ip);
	return WS_Copy(ctx->ws, _mk_Unknown(gir->region), strlen (_mk_Unknown(gir->region)));
}

VCL_STRING
vmod_region_name(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
{
	GeoIPRecord *gir = vmod_geoip_record(ctx, pp, ip);
	VCL_STRING name;

	name = GeoIP_region_name_by_code(gir->country_code, gir->region);

	return WS_Copy(ctx->ws, _mk_Unknown(name), strlen (_mk_Unknown(name)));
}

VCL_STRING
vmod_city(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
{
	GeoIPRecord *gir = vmod_geoip_record(ctx, pp, ip);
	return WS_Copy(ctx->ws, _mk_Unknown(gir->city), strlen (_mk_Unknown(gir->city)));
}

VCL_STRING
vmod_postal_code(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
{
	GeoIPRecord *gir = vmod_geoip_record(ctx, pp, ip);
	return WS_Copy(ctx->ws, _mk_Unknown(gir->postal_code), strlen (_mk_Unknown(gir->postal_code)));
}

VCL_STRING
vmod_longitude(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
{
	char output[50];
	GeoIPRecord *gir = vmod_geoip_record(ctx, pp, ip);

	snprintf(output, 50, "%f", gir->longitude);
	return WS_Copy(ctx->ws, output, strlen (output));
}

VCL_STRING
vmod_latitude(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
{
	char output[50];
	GeoIPRecord *gir = vmod_geoip_record(ctx, pp, ip);

	snprintf(output, 50, "%f", gir->latitude);
	return WS_Copy(ctx->ws, output, strlen (output));
}

// VCL_VOID
// geocode(const struct vrt_ctx *ctx, struct vmod_priv *pp, VCL_STRING ip)
// {
// 	GeoIPRecord *gir = vmod_geoip_record(ctx, pp, ip);

//     VRT_SetHdr(ctx, HDR_RESP, "\014X-Geo-City:", _mk_Unknown(gir->city), vrt_magic_string_end);
//     VRT_SetHdr(ctx, HDR_RESP, "\015X-Geo-Region-Code:", _mk_Unknown(gir->region), vrt_magic_string_end);
//     VRT_SetHdr(ctx, HDR_RESP, "\013X-Geo-Postal-Code:", _mk_Unknown(gir->postal_code), vrt_magic_string_end);
//     VRT_SetHdr(ctx, HDR_RESP, "\013X-Geo-Latitude:", _mk_Unknown(gir->latitude), vrt_magic_string_end);
//     VRT_SetHdr(ctx, HDR_RESP, "\014X-Geo-Longitude:", _mk_Unknown(gir->longitude), vrt_magic_string_end);
//     VRT_SetHdr(ctx, HDR_RESP, "\014X-Geo-IP-Address:", _mk_Unknown(ip), vrt_magic_string_end);
//     VRT_SetHdr(ctx, HDR_RESP, "\014X-Geo-Region-Name:", _mk_Unknown(ip), vrt_magic_string_end);
//     VRT_SetHdr(ctx, HDR_RESP, "\014X-Geo-Country-Code:", _mk_Unknown(gir->country_code), vrt_magic_string_end);
//     VRT_SetHdr(ctx, HDR_RESP, "\014X-Geo-Country-Name:", _mk_Unknown(gir->country_name), vrt_magic_string_end);
// }
